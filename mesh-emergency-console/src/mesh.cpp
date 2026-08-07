// ============================================================
// mesh.cpp - Mesh 组网协议层
// ============================================================

#include "mesh.h"

#include <Arduino.h>
#include <Preferences.h>
#include <esp_mac.h>
#include <cstring>

#include "gnss.h"
#include "radio.h"

// ---------------- 帧格式 ----------------
//  [0]   MAGIC  0xA5
//  [1]   FLAGS  bit0=加密  bit1=SOS  bit2=含定位
//  [2]   TYPE   (MsgType)
//  [3]   SEQ    滚动序号
//  [4..7]SRC_ID 发送方设备编号 (小端)
//  [8]   BODY_LEN
//  [9..] BODY   (加密时整段 AES-CBC)
//  [9+LEN..+1]  CRC16 (覆盖以上全部字节)
//
//  BODY: [NAME_LEN][NAME][DATA_LEN][DATA]
//  TEXT:     DATA = UTF-8 文本
//  LOCATION: DATA = int32 latE7 + int32 lonE7
//  SOS:      DATA = int32 latE7 + int32 lonE7
//  BEACON:   DATA = int32 latE7 + int32 lonE7 + uint8 fix
//  SOS_END:  DATA = 空
// ---------------- 常量 ----------------

static constexpr uint8_t  FRAME_MAGIC   = 0xA5;
static constexpr uint8_t  FLAG_ENCRYPTED = 0x01;
static constexpr uint8_t  FLAG_SOS       = 0x02;
static constexpr uint8_t  FLAG_LOC       = 0x04;
static constexpr size_t   FRAME_HEADER  = 9;
static constexpr uint16_t CRC_INIT      = 0xFFFF;

// ---------------- 本机身份与设置 ----------------
static uint32_t s_selfId    = 0;
static char     s_selfName[NAME_MAX_LEN + 1] = {0};
static char     s_aesKeyStr[32] = {0};
static float    s_freq  = LORA_FREQ_MHZ;
static int8_t   s_power = LORA_TX_POWER;
static uint8_t  s_sf    = LORA_SF;
static uint8_t  s_syncWord = LORA_SYNC_WORD;
static uint8_t  s_seq   = 0;

// ---------------- 设备表 ----------------
static DeviceEntry s_devices[MAX_DEVICES];
static size_t      s_deviceCount = 0;

// ---------------- 消息环形缓冲 ----------------
static ChatMsg s_msgs[MAX_MESSAGES];
static uint8_t s_msgHead  = 0;
static size_t  s_msgCount = 0;

// ---------------- 定时发送状态 ----------------
static uint32_t s_lastBeaconMs = 0;
static uint32_t s_lastSosMs    = 0;
static bool     s_sosLocal     = false;

// ---------------- 求救告警（收到求救信号） ----------------
static bool     s_sosAlertActive = false;
static uint32_t s_sosAlertSource = 0;

// ---------------- CRC16/CCITT ----------------
static uint16_t crc16(const uint8_t* data, size_t len) {
  uint16_t crc = CRC_INIT;
  for (size_t i = 0; i < len; i++) {
    crc ^= (uint16_t)data[i] << 8;
    for (int b = 0; b < 8; b++) {
      crc = (crc & 0x8000) ? (uint16_t)((crc << 1) ^ 0x1021) : (uint16_t)(crc << 1);
    }
  }
  return crc;
}

static void putU32(uint8_t* p, uint32_t v) {
  p[0] = (uint8_t)(v & 0xFF);
  p[1] = (uint8_t)((v >> 8) & 0xFF);
  p[2] = (uint8_t)((v >> 16) & 0xFF);
  p[3] = (uint8_t)((v >> 24) & 0xFF);
}

static uint32_t getU32(const uint8_t* p) {
  return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

// ---------------- 本机 ID ----------------
static uint32_t deriveSelfId() {
  uint8_t mac[6] = {0};
  esp_read_mac(mac, ESP_MAC_WIFI_STA);
  return (uint32_t)mac[3] << 24 | (uint32_t)mac[4] << 16 |
         (uint32_t)mac[5] << 8 | (uint32_t)mac[2];
}

static void defaultName() {
  snprintf(s_selfName, sizeof(s_selfName), "MESH-%04X", s_selfId & 0xFFFF);
}

// ---------------- 初始化 ----------------
bool meshInit() {
  s_selfId = deriveSelfId();

  Preferences prefs;
  prefs.begin("meshnet", false);

  String name = prefs.getString("name", "");
  if (name.length() > 0) {
    strncpy(s_selfName, name.c_str(), NAME_MAX_LEN);
    s_selfName[NAME_MAX_LEN] = '\0';
  } else {
    defaultName();
  }

  String key = prefs.getString("key", "");
  if (key.length() > 0) {
    strncpy(s_aesKeyStr, key.c_str(), sizeof(s_aesKeyStr) - 1);
    radioSetKey(s_aesKeyStr);
  }

  uint32_t freqKhz = prefs.getUInt("freq", (uint32_t)(LORA_FREQ_MHZ * 1000));
  s_freq  = (float)freqKhz / 1000.0f;
  s_power = (int8_t)prefs.getInt("power", LORA_TX_POWER);
  s_sf    = (uint8_t)prefs.getUInt("sf", LORA_SF);
  s_syncWord = (uint8_t)prefs.getUInt("sw", LORA_SYNC_WORD);
  prefs.end();

  radioSetFrequency(s_freq);
  radioSetPower(s_power);
  radioSetSF(s_sf);
  radioSetSyncWord(s_syncWord);
  return true;
}

uint32_t meshSelfId() { return s_selfId; }
const char* meshSelfName() { return s_selfName; }
const char* meshGetKey() { return s_aesKeyStr; }
float meshGetFreq() { return s_freq; }
int8_t meshGetPower() { return s_power; }
uint8_t meshGetSf() { return s_sf; }
uint8_t meshGetSyncWord() { return s_syncWord; }

void meshSetName(const char* name) {
  if (!name || name[0] == '\0') return;
  strncpy(s_selfName, name, NAME_MAX_LEN);
  s_selfName[NAME_MAX_LEN] = '\0';
  Preferences prefs;
  prefs.begin("meshnet", false);
  prefs.putString("name", s_selfName);
  prefs.end();
}

void meshSetAesKey(const char* key) {
  if (!key) return;
  strncpy(s_aesKeyStr, key, sizeof(s_aesKeyStr) - 1);
  s_aesKeyStr[sizeof(s_aesKeyStr) - 1] = '\0';
  radioSetKey(s_aesKeyStr);
  Preferences prefs;
  prefs.begin("meshnet", false);
  prefs.putString("key", s_aesKeyStr);
  prefs.end();
}

void meshSetFreq(float mhz) {
  s_freq = mhz;
  radioSetFrequency(mhz);
  Preferences prefs;
  prefs.begin("meshnet", false);
  prefs.putUInt("freq", (uint32_t)(mhz * 1000));
  prefs.end();
}

void meshSetPower(int8_t dbm) {
  s_power = dbm;
  radioSetPower(dbm);
  Preferences prefs;
  prefs.begin("meshnet", false);
  prefs.putInt("power", dbm);
  prefs.end();
}

void meshSetSf(uint8_t sf) {
  s_sf = sf;
  radioSetSF(sf);
  Preferences prefs;
  prefs.begin("meshnet", false);
  prefs.putUInt("sf", sf);
  prefs.end();
}

void meshSetSyncWord(uint8_t sw) {
  s_syncWord = sw;
  radioSetSyncWord(sw);
  Preferences prefs;
  prefs.begin("meshnet", false);
  prefs.putUInt("sw", sw);
  prefs.end();
}

// ---------------- 设备表 ----------------
static DeviceEntry* findDevice(uint32_t id) {
  for (size_t i = 0; i < s_deviceCount; i++) {
    if (s_devices[i].id == id) return &s_devices[i];
  }
  return nullptr;
}

static DeviceEntry* addDevice(uint32_t id, uint32_t now) {
  if (id == 0) return nullptr;
  if (s_deviceCount >= MAX_DEVICES) {
    // 表满：淘汰最旧的离线设备
    for (size_t i = 0; i < s_deviceCount; i++) {
      if (!s_devices[i].online) {
        s_devices[i] = DeviceEntry();
        s_devices[i].id = id;
        s_devices[i].lastSeenMs = now;
        return &s_devices[i];
      }
    }
    return nullptr;
  }
  DeviceEntry* e = &s_devices[s_deviceCount++];
  e->id = id;
  e->lastSeenMs = now;
  return e;
}

static void pushRssiHist(DeviceEntry* e, int8_t rssi) {
  if (e->rssiHistLen < 16) {
    e->rssiHist[e->rssiHistLen++] = rssi;
  } else {
    memmove(e->rssiHist, e->rssiHist + 1, 15);
    e->rssiHist[15] = rssi;
  }
}

static void touchDevice(uint32_t id, float rssi, float snr, uint32_t now) {
  DeviceEntry* e = findDevice(id);
  if (!e) e = addDevice(id, now);
  if (!e) return;
  e->lastSeenMs = now;
  e->online     = true;
  e->lastRssi   = rssi;
  e->lastSnr    = snr;
  pushRssiHist(e, (int8_t)rssi);
}

// ---------------- 消息缓冲 ----------------
static void pushMessage(const ChatMsg& m) {
  s_msgs[s_msgHead] = m;
  s_msgHead = (uint8_t)((s_msgHead + 1) % MAX_MESSAGES);
  if (s_msgCount < MAX_MESSAGES) s_msgCount++;
}

void meshImportMessage(const ChatMsg& m) {
  pushMessage(m);
}

size_t meshMessageCount() { return s_msgCount; }

const ChatMsg& meshMessageAt(size_t i) {
  if (i >= s_msgCount) i = s_msgCount - 1;
  size_t idx = (size_t)(s_msgHead - s_msgCount + i + MAX_MESSAGES) % MAX_MESSAGES;
  return s_msgs[idx];
}

// ---------------- 设备表访问 ----------------
size_t meshDeviceCount() { return s_deviceCount; }
const DeviceEntry* meshDevices() { return s_devices; }

bool meshFindDeviceName(uint32_t id, char* out, size_t cap) {
  DeviceEntry* e = findDevice(id);
  if (!e) return false;
  if (e->name[0]) {
    strncpy(out, e->name, cap - 1);
    out[cap - 1] = '\0';
  } else {
    char idbuf[9];
    snprintf(idbuf, sizeof(idbuf), "%08X", (unsigned)id);
    strncpy(out, idbuf, cap - 1);
    out[cap - 1] = '\0';
  }
  return true;
}

// ---------------- 帧构造与发送 ----------------
static bool buildAndSend(uint8_t type, const uint8_t* data, size_t dataLen) {
  // body = [NAME_LEN][NAME][DATA_LEN][DATA]
  uint8_t body[RADIO_MAX_FRAME];
  size_t  nameLen = (size_t)strlen(s_selfName);
  size_t  p = 0;
  body[p++] = (uint8_t)nameLen;
  memcpy(body + p, s_selfName, nameLen); p += nameLen;
  body[p++] = (uint8_t)dataLen;
  memcpy(body + p, data, dataLen);       p += dataLen;

  uint8_t frame[RADIO_MAX_FRAME];
  size_t  f = 0;
  frame[f++] = FRAME_MAGIC;
  uint8_t flags = 0;
  if (type == MSG_SOS) flags |= FLAG_SOS;

  // 尝试加密整段 body
  size_t bodyLen = p;
  uint8_t enc[RADIO_MAX_FRAME];
  size_t encLen  = 0;
  bool encrypted = cryptoEncrypt(body, bodyLen, enc, &encLen);
  const uint8_t* bodyOut = body;
  size_t bodyOutLen = bodyLen;
  if (encrypted) {
    flags |= FLAG_ENCRYPTED;
    bodyOut = enc;
    bodyOutLen = encLen;
  }
  if (type == MSG_LOCATION || type == MSG_SOS || type == MSG_BEACON) {
    flags |= FLAG_LOC;
  }

  frame[f++] = flags;
  frame[f++] = type;
  frame[f++] = s_seq++;
  putU32(frame + f, s_selfId); f += 4;
  frame[f++] = (uint8_t)bodyOutLen;
  memcpy(frame + f, bodyOut, bodyOutLen); f += bodyOutLen;

  uint16_t crc = crc16(frame, f);
  frame[f++] = (uint8_t)(crc & 0xFF);
  frame[f++] = (uint8_t)(crc >> 8);

  return radioSend(frame, f);
}

static void sendText(const char* text) {
  size_t len = strlen(text);
  if (len == 0) return;
  if (len > MSG_MAX_LEN) len = MSG_MAX_LEN;
  buildAndSend(MSG_TEXT, (const uint8_t*)text, len);
}

static void sendLocation(float lat, float lon) {
  int32_t latE7 = (int32_t)(lat * 1e7f);
  int32_t lonE7 = (int32_t)(lon * 1e7f);
  uint8_t data[8];
  putU32(data, (uint32_t)latE7);
  putU32(data + 4, (uint32_t)lonE7);
  buildAndSend(MSG_LOCATION, data, 8);
}

static void sendSos() {
  uint8_t data[8] = {0};
  const GnssData& g = gnssData();
  if (g.valid) {
    putU32(data, (uint32_t)(int32_t)(g.lat * 1e7f));
    putU32(data + 4, (uint32_t)(int32_t)(g.lon * 1e7f));
  }
  buildAndSend(MSG_SOS, data, 8);
}

// ---------------- 帧解析 ----------------
static void handleDecodedFrame(uint8_t type, uint32_t srcId, const char* name,
                               const uint8_t* data, size_t dataLen,
                               bool encrypted, float rssi, float snr, uint32_t now);

void meshOnFrame(const uint8_t* buf, size_t len, float rssi, float snr, uint32_t now) {
  if (len < FRAME_HEADER + 2) return;
  if (buf[0] != FRAME_MAGIC) return;

  // CRC 校验
  uint16_t crcGot = (uint16_t)buf[len - 2] | ((uint16_t)buf[len - 1] << 8);
  if (crc16(buf, len - 2) != crcGot) return;

  uint8_t flags = buf[1];
  uint8_t type  = buf[2];
  uint32_t srcId = getU32(buf + 4);
  size_t bodyLen = buf[8];

  if (9 + bodyLen + 2 != len) return;
  if (srcId == s_selfId) return;  // 忽略自己

  bool encrypted = (flags & FLAG_ENCRYPTED) != 0;

  uint8_t body[RADIO_MAX_FRAME];
  size_t  bodyOutLen = bodyLen;
  const uint8_t* bodyP = buf + 9;

  if (encrypted) {
    if (!cryptoDecrypt(buf + 9, bodyLen, body, &bodyOutLen)) {
      // 密钥不一致：只能记录"空白报文"与设备存在
      if ((flags & FLAG_SOS) != 0 && type == MSG_SOS) {
        s_sosAlertActive = true;
        s_sosAlertSource = srcId;
        DeviceEntry* e = findDevice(srcId);
        if (!e) e = addDevice(srcId, now);
        if (e) { e->sosSeen = true; }
      }
      handleDecodedFrame(type, srcId, nullptr, nullptr, 0, true, rssi, snr, now);
      return;
    }
    bodyP = body;
  }

  // 解析 body
  if (bodyOutLen < 1) { handleDecodedFrame(type, srcId, nullptr, nullptr, 0, encrypted, rssi, snr, now); return; }
  size_t nameLen = bodyP[0];
  size_t p = 1;
  char name[NAME_MAX_LEN + 1] = {0};
  if (nameLen > NAME_MAX_LEN) nameLen = NAME_MAX_LEN;
  if (p + nameLen <= bodyOutLen) {
    memcpy(name, bodyP + p, nameLen); name[nameLen] = '\0';
    p += nameLen;
  }
  const uint8_t* data = nullptr;
  size_t dataLen = 0;
  if (p < bodyOutLen) {
    dataLen = bodyP[p];
    p += 1;
    if (p + dataLen <= bodyOutLen) data = bodyP + p;
    else dataLen = 0;
  }

  handleDecodedFrame(type, srcId, name, data, dataLen, encrypted, rssi, snr, now);
}

static void handleDecodedFrame(uint8_t type, uint32_t srcId, const char* name,
                               const uint8_t* data, size_t dataLen,
                               bool encrypted, float rssi, float snr, uint32_t now) {
  // 更新设备表（存在感 + 信号）
  touchDevice(srcId, rssi, snr, now);
  DeviceEntry* e = findDevice(srcId);
  if (e && name && name[0]) {
    strncpy(e->name, name, NAME_MAX_LEN);
    e->name[NAME_MAX_LEN] = '\0';
  }

  float lat = 0, lon = 0;
  bool  hasLoc = false;
  if (data && dataLen >= 8) {
    lat = (float)(int32_t)getU32(data) / 1e7f;
    lon = (float)(int32_t)getU32(data + 4) / 1e7f;
    hasLoc = true;
    if (e) { e->lat = lat; e->lon = lon; e->hasLocation = true; }
  }

  ChatMsg m;
  m.incoming = true;
  m.type     = type;
  m.srcId    = srcId;
  m.rxMs     = now;
  m.rssi     = rssi;
  m.encrypted = encrypted;
  m.hasLocation = hasLoc;
  m.lat = lat; m.lon = lon;
  if (name) strncpy(m.name, name, NAME_MAX_LEN);

  switch (type) {
    case MSG_TEXT:
      if (!encrypted && data && dataLen > 0) {
        size_t tl = dataLen < MSG_MAX_LEN ? dataLen : MSG_MAX_LEN;
        memcpy(m.text, data, tl);
        m.text[tl] = '\0';
      } else if (encrypted) {
        strncpy(m.text, "<加密消息>", MSG_MAX_LEN);
      }
      pushMessage(m);
      break;

    case MSG_LOCATION:
      strncpy(m.text, "[位置] 已收到坐标", MSG_MAX_LEN);
      pushMessage(m);
      break;

    case MSG_SOS:
      strncpy(m.text, hasLoc ? "[求救] 发送了位置坐标" : "[求救] 未定位", MSG_MAX_LEN);
      pushMessage(m);
      s_sosAlertActive = true;
      s_sosAlertSource = srcId;
      if (e) e->sosSeen = true;
      break;

    case MSG_SOS_END:
      if (e) e->sosSeen = false;
      if (s_sosAlertSource == srcId) s_sosAlertActive = false;
      break;

    case MSG_BEACON:
    default:
      // 心跳：仅更新设备表，不产生聊天记录
      break;
  }
}

// ---------------- 对外发送 ----------------
void meshSendText(const char* text) {
  if (meshSosActive()) return;  // 求救模式优先
  sendText(text);
  // 记录本机发出的消息
  ChatMsg m;
  m.incoming = false;
  m.type     = MSG_TEXT;
  m.srcId    = s_selfId;
  m.rxMs     = millis();
  strncpy(m.name, s_selfName, NAME_MAX_LEN);
  size_t tl = strlen(text);
  if (tl > MSG_MAX_LEN) tl = MSG_MAX_LEN;
  memcpy(m.text, text, tl);
  m.text[tl] = '\0';
  pushMessage(m);
}

void meshSendLocation(float lat, float lon) {
  if (meshSosActive()) return;
  sendLocation(lat, lon);
  ChatMsg m;
  m.incoming = false;
  m.type     = MSG_LOCATION;
  m.srcId    = s_selfId;
  m.rxMs     = millis();
  strncpy(m.name, s_selfName, NAME_MAX_LEN);
  strncpy(m.text, "[位置] 已发送坐标", MSG_MAX_LEN);
  m.lat = lat; m.lon = lon; m.hasLocation = true;
  pushMessage(m);
}

void meshSendSosNow() { sendSos(); }
void meshSendSosStopNow() { buildAndSend(MSG_SOS_END, nullptr, 0); }

void meshStartSos() {
  s_sosLocal = true;
  s_lastSosMs = 0;
  meshSendSosNow();
  ChatMsg m;
  m.incoming = false; m.type = MSG_SOS; m.srcId = s_selfId; m.rxMs = millis();
  strncpy(m.name, s_selfName, NAME_MAX_LEN);
  strncpy(m.text, "[求救] 已开始广播求救信号", MSG_MAX_LEN);
  pushMessage(m);
}

void meshStopSos() {
  if (!s_sosLocal) return;
  s_sosLocal = false;
  meshSendSosStopNow();
  ChatMsg m;
  m.incoming = false; m.type = MSG_SOS_END; m.srcId = s_selfId; m.rxMs = millis();
  strncpy(m.name, s_selfName, NAME_MAX_LEN);
  strncpy(m.text, "[求救] 已停止广播", MSG_MAX_LEN);
  pushMessage(m);
}

bool meshSosActive() { return s_sosLocal; }

// ---------------- 求救告警 ----------------
bool meshSosAlertActive() { return s_sosAlertActive; }
uint32_t meshSosAlertSource() { return s_sosAlertSource; }
void meshAckSos() { s_sosAlertActive = false; }

// ---------------- 周期任务 ----------------
void meshTick(uint32_t now) {
  // 心跳广播
  if (now - s_lastBeaconMs >= DEVICE_BEACON_INTERVAL_S * 1000UL) {
    s_lastBeaconMs = now;
    uint8_t data[9] = {0};
    const GnssData& g = gnssData();
    if (g.valid) {
      putU32(data, (uint32_t)(int32_t)(g.lat * 1e7f));
      putU32(data + 4, (uint32_t)(int32_t)(g.lon * 1e7f));
      data[8] = 1;
    }
    buildAndSend(MSG_BEACON, data, 9);
  }

  // 求救循环广播
  if (s_sosLocal && now - s_lastSosMs >= SOS_BROADCAST_INTERVAL_MS) {
    s_lastSosMs = now;
    sendSos();
  }

  // 设备离线判定
  for (size_t i = 0; i < s_deviceCount; i++) {
    if (s_devices[i].online &&
        now - s_devices[i].lastSeenMs > DEVICE_TIMEOUT_MS) {
      s_devices[i].online = false;
    }
  }
}
