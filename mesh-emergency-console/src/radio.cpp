// ============================================================
// radio.cpp - LoRa (SX1262) 无线链路 + AES 加密原语
// ============================================================

#include "radio.h"

#include <M5Unified.h>
#include <SPI.h>
#include <RadioLib.h>
#include <mbedtls/aes.h>
#include <mbedtls/sha256.h>
#include <cstring>

#include "utility/PI4IOE5V6408_Class.hpp"
#include "config.h"

// ---- 共享 SPI 总线（LoRa 与 SD 卡共用 SCK/MOSI/MISO） ----
static SPIClass* s_spi = nullptr;

// ---- RadioLib 设备 ----
static Module* s_module = nullptr;
static SX1262* s_lora   = nullptr;

// ---- 中断标志（ISR 中置位） ----
static volatile bool s_rxFlag = false;
static volatile bool s_txFlag = false;

#if defined(ESP32) || defined(ESP8266)
ICACHE_RAM_ATTR
#endif
static void onRadioIRQ(void) {
  s_rxFlag = true;
}

#if defined(ESP32) || defined(ESP8266)
ICACHE_RAM_ATTR
#endif
static void onRadioTxIRQ(void) {
  s_txFlag = true;
}

static RadioStatus s_status;

// ---- 接收缓冲 ----
static uint8_t s_rxBuf[RADIO_MAX_FRAME];
static size_t  s_rxLen   = 0;
static float   s_rxRssi  = 0.0f;
static float   s_rxSnr   = 0.0f;
static bool    s_rxReady = false;

// ---- 待发送队列（忙时暂存一帧，发送完成后自动补发） ----
static uint8_t s_pendingBuf[RADIO_MAX_FRAME];
static size_t  s_pendingLen = 0;
static bool    s_pendingValid = false;

// ---- AES 密钥（由用户密钥哈希派生） ----
static uint8_t s_aesKey[16];
static uint8_t s_aesIv[16];
static bool    s_keyValid = false;

// ------------------------------------------------------------
// AES
// ------------------------------------------------------------
void radioSetKey(const char* key) {
  if (key == nullptr || key[0] == '\0') {
    s_keyValid = false;
    return;
  }
  uint8_t hash[32];
  mbedtls_sha256((const uint8_t*)key, strlen(key), hash, 0);
  memcpy(s_aesKey, hash, 16);
  memcpy(s_aesIv, hash + 16, 16);
  s_keyValid = true;
}

bool cryptoEncrypt(const uint8_t* plain, size_t plen, uint8_t* cipher, size_t* clen) {
  if (!s_keyValid) return false;
  size_t pad  = 16 - (plen % 16);
  size_t total = plen + pad;
  uint8_t buf[RADIO_MAX_FRAME];
  memcpy(buf, plain, plen);
  memset(buf + plen, (int)pad, pad);  // PKCS7

  uint8_t iv[16];
  memcpy(iv, s_aesIv, 16);
  mbedtls_aes_context ctx;
  mbedtls_aes_init(&ctx);
  int ret = mbedtls_aes_setkey_enc(&ctx, s_aesKey, 128);
  if (ret != 0) { mbedtls_aes_free(&ctx); return false; }
  ret = mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, total, iv, buf, cipher);
  mbedtls_aes_free(&ctx);
  if (ret != 0) return false;
  *clen = total;
  return true;
}

bool cryptoDecrypt(const uint8_t* cipher, size_t clen, uint8_t* plain, size_t* plen) {
  if (!s_keyValid) return false;
  if (clen == 0 || clen % 16 != 0) return false;

  uint8_t buf[RADIO_MAX_FRAME];
  uint8_t iv[16];
  memcpy(iv, s_aesIv, 16);
  mbedtls_aes_context ctx;
  mbedtls_aes_init(&ctx);
  int ret = mbedtls_aes_setkey_dec(&ctx, s_aesKey, 128);
  if (ret != 0) { mbedtls_aes_free(&ctx); return false; }
  ret = mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_DECRYPT, clen, iv, cipher, buf);
  mbedtls_aes_free(&ctx);
  if (ret != 0) return false;

  // PKCS7 去填充
  uint8_t pad = buf[clen - 1];
  if (pad == 0 || pad > 16) return false;
  size_t outLen = clen - pad;
  memcpy(plain, buf, outLen);
  *plen = outLen;
  return true;
}

// ------------------------------------------------------------
// 射频开关（Cap LoRa-1262 需使能 PI4IOE5V6408 的 P0 天线开关）
// ------------------------------------------------------------
static bool initRfSwitch(void) {
  m5::PI4IOE5V6408_Class ioe(0x43, 400000, &m5::In_I2C);
  if (!ioe.begin()) {
    // 可能是无射频开关的 Cap LoRa868 早期版本，无需使能
    return false;
  }
  ioe.setDirection(0, true);          // P0 -> 输出
  ioe.setHighImpedance(0, false);     // 关闭高阻，引脚可驱动
  ioe.digitalWrite(0, true);          // 置高，使能 RF 天线开关
  return true;
}

SPIClass& radioSPI() {
  return *s_spi;
}

// ------------------------------------------------------------
// 初始化
// ------------------------------------------------------------
bool radioInit() {
  // 共享 SPI 总线（与 SD 卡共用）
  s_spi = new SPIClass(FSPI);
  s_spi->begin(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI, PIN_SD_CS);

  s_status.rfSwitchOk = initRfSwitch();

  s_module = new Module(PIN_LORA_NSS, PIN_LORA_IRQ, PIN_LORA_RST, PIN_LORA_BUSY,
                        *s_spi, SPISettings(4000000, MSBFIRST, SPI_MODE0));
  s_lora   = new SX1262(s_module);

  int state = s_lora->begin(LORA_FREQ_MHZ, LORA_BW_KHZ, LORA_SF, LORA_CR,
                            LORA_SYNC_WORD, LORA_TX_POWER, LORA_PREAMBLE,
                            3.0f, true);  // TCXO 3.0V
  s_status.lastState = (uint8_t)state;
  s_status.radioOk   = (state == RADIOLIB_ERR_NONE);
  if (!s_status.radioOk) {
    return false;
  }

  s_lora->setCurrentLimit(LORA_CURRENT_LIMIT_MA);
  s_lora->setPacketSentAction(onRadioTxIRQ);
  s_lora->setPacketReceivedAction(onRadioIRQ);

  s_status.freqMHz  = LORA_FREQ_MHZ;
  s_status.txPower  = LORA_TX_POWER;
  s_status.sf       = LORA_SF;
  s_status.syncWord = LORA_SYNC_WORD;

  state = s_lora->startReceive();
  s_status.txBusy = false;
  return (state == RADIOLIB_ERR_NONE);
}

// ------------------------------------------------------------
// 每帧轮询
// ------------------------------------------------------------
void radioPoll() {
  if (!s_status.radioOk) return;

  // 发送完成：补发待发帧，否则关闭发射恢复接收
  if (s_txFlag) {
    s_txFlag = false;
    s_lora->finishTransmit();
    s_status.txBusy = false;
    if (s_pendingValid) {
      size_t len = s_pendingLen;
      s_pendingValid = false;
      s_status.txBusy = true;
      if (s_lora->startTransmit(s_pendingBuf, len) == RADIOLIB_ERR_NONE) {
        return;  // 正在发送补发帧，等待其完成
      }
      s_status.txBusy = false;
    }
    s_lora->startReceive();
  }

  // 收到一帧
  if (s_rxFlag) {
    s_rxFlag = false;
    int state = s_lora->readData(s_rxBuf, RADIO_MAX_FRAME);
    if (state >= 0) {
      s_rxLen   = (size_t)state;
      s_rxRssi  = s_lora->getRSSI();
      s_rxSnr   = s_lora->getSNR();
      s_rxReady = true;
    }
    s_lora->startReceive();  // 继续监听
  }
}

bool radioHasFrame() {
  return s_rxReady;
}

bool radioReadFrame(uint8_t* buf, size_t* len, float* rssi, float* snr) {
  if (!s_rxReady) return false;
  memcpy(buf, s_rxBuf, s_rxLen);
  *len = s_rxLen;
  if (rssi) *rssi = s_rxRssi;
  if (snr)  *snr  = s_rxSnr;
  s_rxReady = false;
  return true;
}

bool radioSend(const uint8_t* buf, size_t len) {
  if (!s_status.radioOk || len == 0 || len > RADIO_MAX_FRAME) return false;
  if (s_status.txBusy) {
    // 忙：暂存一帧，发送完成后自动补发
    memcpy(s_pendingBuf, buf, len);
    s_pendingLen = len;
    s_pendingValid = true;
    return true;
  }
  s_status.txBusy = true;
  int state = s_lora->startTransmit(buf, len);
  if (state != RADIOLIB_ERR_NONE) {
    s_status.txBusy = false;
    return false;
  }
  return true;
}

// ------------------------------------------------------------
// 参数调整
// ------------------------------------------------------------
bool radioSetFrequency(float mhz) {
  if (!s_status.radioOk) return false;
  if (s_lora->setFrequency(mhz) == RADIOLIB_ERR_NONE) {
    s_status.freqMHz = mhz;
    return true;
  }
  return false;
}

bool radioSetPower(int8_t dbm) {
  if (!s_status.radioOk) return false;
  if (s_lora->setOutputPower(dbm) == RADIOLIB_ERR_NONE) {
    s_status.txPower = dbm;
    return true;
  }
  return false;
}

bool radioSetSF(uint8_t sf) {
  if (!s_status.radioOk) return false;
  if (s_lora->setSpreadingFactor(sf) == RADIOLIB_ERR_NONE) {
    s_status.sf = sf;
    return true;
  }
  return false;
}

bool radioSetSyncWord(uint8_t sw) {
  if (!s_status.radioOk) return false;
  if (s_lora->setSyncWord(sw) == RADIOLIB_ERR_NONE) {
    s_status.syncWord = sw;
    s_lora->startReceive();  // 同步字变更后回到监听状态
    return true;
  }
  return false;
}

const RadioStatus& radioGetStatus() {
  return s_status;
}
