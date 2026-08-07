#pragma once

// ============================================================
// store.h - SD 卡持久化
//   聊天记录日志 + 坐标点（航点）保存与读取。
// SD 不可用时所有函数安全降级（静默失败）。
// ============================================================

#include <cstdint>
#include <cstddef>

#include "mesh.h"

// 初始化 SD 卡（在 radioInit 之后调用，共用 SPI 总线）
bool storeBegin();

// SD 卡是否可用
bool storeAvailable();

// 追加一条消息到历史日志
void storeLogMessage(const ChatMsg& m);

// 启动时加载最近的历史消息到消息缓冲（最多 maxCount 条）
void storeLoadHistory(size_t maxCount);

// ---- 航点（坐标点记录） ----
bool   storeAddWaypoint(const char* name, float lat, float lon);
size_t storeWaypointCount();
bool   storeWaypointAt(size_t i, char* name, size_t nameCap, float* lat, float* lon);
