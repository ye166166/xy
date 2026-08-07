#pragma once

// ============================================================
// radio.h - LoRa (SX1262) 无线链路 + AES 加密原语
// 封装 RadioLib 的收发、Cap LoRa-1262 的 RF 天线开关，
// 以及基于 mbedtls 的 AES-128-CBC 加解密（PKCS7 填充）。
// ============================================================

#include <cstdint>
#include <cstddef>
#include <SPI.h>

constexpr size_t RADIO_MAX_FRAME = 240;  // 单帧最大字节（含帧头/CRC）

// 无线链路状态快照（供 UI 显示）
struct RadioStatus {
  bool  radioOk      = false;   // SX1262 初始化是否成功
  bool  rfSwitchOk   = false;   // RF 天线开关（PI4IOE5V6408）是否就绪
  bool  txBusy       = false;   // 正在发送
  float freqMHz      = 0.0f;    // 当前载波频率
  int8_t txPower     = 0;       // 当前发射功率
  uint8_t sf         = 0;       // 当前扩频因子
  uint8_t syncWord   = 0;       // 当前信道同步字
  uint8_t lastState  = 0;       // 最近一次 begin 的返回码
};

// 初始化：SPI、RF 开关、SX1262（需要在 M5Cardputer.begin 之后调用）
bool radioInit();

// 共享 SPI 总线实例（LoRa 与 SD 卡共用，供其他模块复用）
SPIClass& radioSPI();

// 每帧调用：处理接收/发送完成中断，得到最新一帧
void radioPoll();

// 是否收到一帧完整数据
bool radioHasFrame();

// 取走最新一帧（返回帧长度；rssi/snr 为该帧的链路指标）
bool radioReadFrame(uint8_t* buf, size_t* len, float* rssi, float* snr);

// 发送一帧原始字节（异步，busy 期间调用会返回 false）
bool radioSend(const uint8_t* buf, size_t len);

// 设置 AES 通讯密钥（仅影响之后收发时对帧体的加解密）
void radioSetKey(const char* key);

// AES-128-CBC 加解密原语（供协议层使用）
// 传入/传出缓冲区需能容纳 PKCS7 填充后的数据（最多 +16 字节）
bool cryptoEncrypt(const uint8_t* plain, size_t plen, uint8_t* cipher, size_t* clen);
bool cryptoDecrypt(const uint8_t* cipher, size_t clen, uint8_t* plain, size_t* plen);

// 参数调整（返回是否成功，成功后 radioGetStatus 同步更新）
bool radioSetFrequency(float mhz);
bool radioSetPower(int8_t dbm);
bool radioSetSF(uint8_t sf);
bool radioSetSyncWord(uint8_t sw);

// 当前链路状态
const RadioStatus& radioGetStatus();
