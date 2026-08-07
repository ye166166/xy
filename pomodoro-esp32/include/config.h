#pragma once

// ============================================================
// 番茄时钟可配置参数
// 改这里即可调整时长、提醒音、晃动灵敏度等，无需改动逻辑代码。
// ============================================================

#include <cstdint>

// ---- 各阶段时长（毫秒） ----
constexpr uint32_t WORK_MS       = 25 * 60 * 1000UL;  // 学习中 25 分钟
constexpr uint32_t SHORT_BREAK_MS = 5 * 60 * 1000UL;  // 短休息  5 分钟
constexpr uint32_t LONG_BREAK_MS  = 20 * 60 * 1000UL; // 长休息 20 分钟

// 累计完成多少个番茄后进入长休息
constexpr uint8_t CYCLE_LENGTH = 4;

// ---- 提醒音 ----
constexpr uint16_t BEEP_FREQ_HZ = 880;    // 提示音频率
constexpr uint16_t BEEP_MS      = 2000;   // 提示音持续时长（要求 2 秒）
constexpr uint8_t  BEEP_VOLUME  = 120;    // 音量 0~255

// ---- BMI270 晃动检测 ----
// 陀螺仪合成角速度超过该阈值视为"大幅晃动"（单位 度/秒）
constexpr float SHAKE_GYRO_THRESHOLD = 350.0f;
// 连续大幅晃动累积满该时长（毫秒）触发暂停
constexpr uint32_t SHAKE_TRIGGER_MS = 3000;
// 触发一次后，防止连续触发的冷却时间
constexpr uint32_t SHAKE_COOLDOWN_MS = 2000;
// 晃动累积量的自然衰减系数（毫秒/毫秒），越大衰减越快
constexpr float SHAKE_DECAY = 0.3f;
