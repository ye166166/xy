#pragma once

// ============================================================
// gnss.h - ATGM336H (Cap LoRa-1262) GNSS 定位
// 轻量 NMEA 0183 解析（$xxRMC / $xxGGA），提取经纬度与卫星数。
// ============================================================

#include <cstdint>

struct GnssData {
  bool     valid   = false;  // 当前是否有效定位（RMC 状态 A / 定位标志）
  float    lat     = 0.0f;   // 纬度（度）
  float    lon     = 0.0f;   // 经度（度）
  uint8_t  sats    = 0;      // 参与定位的卫星数
  bool     updated = false;  // 本帧是否更新了定位（供 UI 判断）
  uint32_t lastFixMs = 0;    // 最近一次有效定位的 millis()
};

// 初始化 GNSS（UART）
bool gnssInit();

// 每帧调用：读取串口并解析
void gnssUpdate();

// 当前定位数据
const GnssData& gnssData();
