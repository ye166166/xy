#pragma once

// ============================================================
// Mesh应急通讯控制台 - 可配置参数
// 修改本文件即可调整射频、组网、UI 等参数，无需改动逻辑代码。
// ============================================================

#include <cstdint>

// ---- 射频参数 (SX1262) ----
// 载波频率 (MHz)。Cap LoRa-1262 支持 868~923MHz，默认取欧盟 ISM 频段。
// 不同地区法规不同，请按当地法规自行调整。
constexpr float LORA_FREQ_MHZ    = 868.0f;
constexpr float LORA_BW_KHZ      = 125.0f;   // 带宽 125kHz
constexpr uint8_t LORA_SF        = 7;        // 扩频因子 7~12，越大越远越慢
constexpr uint8_t LORA_CR        = 5;        // 编码率 4/5
constexpr uint8_t LORA_SYNC_WORD = 0x34;     // 同步字，收发双方须一致
constexpr int8_t  LORA_TX_POWER  = 20;       // 发射功率 (dBm)，2~22 可调
constexpr uint16_t LORA_PREAMBLE = 20;       // 前导码长度
constexpr uint8_t  LORA_CURRENT_LIMIT_MA = 140;  // 限流 (mA)

// ---- LoRa 引脚 (Cardputer-Adv EXT 总线) ----
constexpr uint8_t PIN_LORA_NSS  = 5;
constexpr uint8_t PIN_LORA_IRQ  = 4;   // DIO1
constexpr uint8_t PIN_LORA_RST  = 3;
constexpr uint8_t PIN_LORA_BUSY = 6;

// ---- SPI 总线 (与 SD 卡共用) ----
constexpr uint8_t PIN_SPI_SCK  = 40;
constexpr uint8_t PIN_SPI_MISO = 39;
constexpr uint8_t PIN_SPI_MOSI = 14;
constexpr uint8_t PIN_SD_CS    = 12;

// ---- GNSS (ATGM336H) ----
constexpr uint8_t  PIN_GPS_RX = 15;   // Cardputer UART_RX <- GPS TX
constexpr uint8_t  PIN_GPS_TX = 13;   // Cardputer UART_TX -> GPS RX
constexpr uint32_t GPS_BAUD   = 115200;

// ---- 组网与设备管理 ----
constexpr uint8_t  DEVICE_BEACON_INTERVAL_S = 5;      // 心跳广播间隔(秒)
constexpr uint32_t DEVICE_TIMEOUT_MS         = 30000; // 超过该时长无心跳视为离线
constexpr uint8_t  MAX_DEVICES               = 20;    // 在线设备表上限
constexpr uint8_t  MAX_MESSAGES              = 200;   // 内存保留消息条数(全部落SD)
constexpr uint16_t MSG_MAX_LEN               = 120;   // 单条消息最大字节(UTF-8)
constexpr uint8_t  NAME_MAX_LEN              = 18;    // 设备名最大字节
constexpr uint8_t  WAYPOINT_MAX              = 50;    // 坐标点数量上限

// ---- 求救广播 ----
constexpr uint32_t SOS_BROADCAST_INTERVAL_MS = 2000;  // 求救报文广播间隔
constexpr uint16_t SOS_ALARM_FREQ_HZ         = 980;   // 警报提示音频率
constexpr uint8_t  SOS_ALARM_VOLUME          = 200;   // 警报音量 0~255

// ---- 陀螺仪体感 ----
// 晃动: 连续大幅晃动超过该累积时长即触发"发送位置"
constexpr float    SHAKE_GYRO_THRESHOLD = 400.0f;     // 合成角速度阈值 (deg/s)
constexpr uint32_t SHAKE_TRIGGER_MS     = 700;        // 连续晃动累积毫秒
constexpr uint32_t SHAKE_COOLDOWN_MS    = 3000;       // 触发冷却
// 倾斜: 仰俯角变化超过该阈值(度)即滚动消息列表
constexpr float    TILT_SCROLL_DEG      = 22.0f;      // 倾斜触发阈值
constexpr uint32_t TILT_SCROLL_COOLDOWN = 400;        // 连续滚动间隔
// 陀螺仪不可用时的备用滚动按键间隔(无实际用途, 保留)

// ---- 参数可调范围 (设置面板) ----
constexpr uint8_t  LORA_SF_MIN      = 7;
constexpr uint8_t  LORA_SF_MAX      = 12;
constexpr int8_t   LORA_POWER_MIN   = 2;
constexpr int8_t   LORA_POWER_MAX   = 22;
constexpr float    LORA_FREQ_MIN    = 860.0f;
constexpr float    LORA_FREQ_MAX    = 925.0f;
