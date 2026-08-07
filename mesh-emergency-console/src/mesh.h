#pragma once

// ============================================================
// mesh.h - Mesh 组网协议层
//   帧编解码、设备注册/心跳、消息环形缓冲、本机身份与设置。
// 与硬件无关（radio 层负责收发光字节，UI 层负责展示）。
// ============================================================

#include <cstdint>
#include <cstddef>

#include "config.h"

// 报文类型（帧头明文，保证异组也能感知存在/求救）
enum MsgType : uint8_t {
  MSG_TEXT     = 1,  // 文字消息
  MSG_LOCATION = 2,  // 位置坐标
  MSG_SOS      = 3,  // 紧急求救
  MSG_BEACON   = 4,  // 设备心跳/在线广播
  MSG_SOS_END  = 5,  // 求救结束
};

// 一条消息记录（UI 与 SD 持久化共用）
struct ChatMsg {
  bool     incoming    = false;      // true=收到的, false=本机发出
  uint8_t  type        = MSG_TEXT;
  uint32_t srcId       = 0;          // 发送方设备编号
  uint32_t rxMs        = 0;          // 时间戳 (millis)
  float    rssi        = 0.0f;       // 无线信号强度 dBm（本机发送为 0）
  char     name[NAME_MAX_LEN + 1] = {0};  // 发送方设备名
  char     text[MSG_MAX_LEN + 1]  = {0};  // 正文（加密未解为 "<加密消息>"）
  bool     encrypted   = false;      // 因密钥不一致无法解密
  float    lat         = 0.0f;
  float    lon         = 0.0f;
  bool     hasLocation = false;
};

// 组网内的一台设备
struct DeviceEntry {
  uint32_t id        = 0;
  char     name[NAME_MAX_LEN + 1] = {0};
  uint32_t lastSeenMs = 0;
  float    lastRssi  = 0.0f;
  float    lastSnr   = 0.0f;
  bool     online    = false;
  float    lat       = 0.0f;
  float    lon       = 0.0f;
  bool     hasLocation = false;
  int8_t   rssiHist[16];            // 信号强度历史（动态波形）
  uint8_t  rssiHistLen = 0;
  bool     sosSeen   = false;       // 该设备是否在求救
};

// 初始化：加载本机身份/设置（Preferences NVS）
bool meshInit();

uint32_t   meshSelfId();
const char* meshSelfName();
void       meshSetName(const char* name);
void       meshSetAesKey(const char* key);
float      meshGetFreq();  void meshSetFreq(float mhz);
int8_t     meshGetPower(); void meshSetPower(int8_t dbm);
uint8_t    meshGetSf();    void meshSetSf(uint8_t sf);
uint8_t    meshGetSyncWord(); void meshSetSyncWord(uint8_t sw);
const char* meshGetKey();

// 每帧调用（心跳广播 / 求救广播计时）
void meshTick(uint32_t now);

// 收到一帧原始数据
void meshOnFrame(const uint8_t* buf, size_t len, float rssi, float snr, uint32_t now);

// 对外发送接口
void meshSendText(const char* text);
void meshSendLocation(float lat, float lon);
void meshSendSosNow();           // 发送一次求救帧
void meshSendSosStopNow();       // 发送求救结束帧
void meshStartSos();
void meshStopSos();
bool meshSosActive();

// 收到外部求救信号时的告警状态
bool     meshSosAlertActive();
uint32_t meshSosAlertSource();
void     meshAckSos();

// 消息缓冲访问（0 = 最旧）
size_t meshMessageCount();
const ChatMsg& meshMessageAt(size_t i);

// 导入一条历史消息（启动时从 SD 加载），不发送、不联网
void meshImportMessage(const ChatMsg& m);

// 设备表访问
size_t meshDeviceCount();
const DeviceEntry* meshDevices();
// 查询设备显示名（无名称时回退为十六进制编号），成功返回 true
bool meshFindDeviceName(uint32_t id, char* out, size_t cap);
