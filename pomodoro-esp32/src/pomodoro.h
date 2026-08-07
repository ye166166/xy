#pragma once

#include <cstdint>

#include "config.h"

// 番茄时钟状态机（纯逻辑，不依赖任何硬件）
enum class Phase : uint8_t {
  IDLE,         // 待机：尚未开始
  WORK,         // 学习中
  SHORT_BREAK,  // 短休息
  LONG_BREAK,   // 长休息
};

struct PomodoroState {
  Phase    phase = Phase::IDLE;
  bool     running = false;   // 是否正在倒计时
  bool     beeping = false;   // 正在响铃（阶段结束）
  uint32_t remainingMs = WORK_MS;   // 当前阶段剩余毫秒
  uint32_t lastTickMs = 0;          // 上次 tick 的 millis
  uint32_t beepUntilMs = 0;         // 响铃结束时刻
  uint8_t  completedInCycle = 0;    // 本周期已完成番茄数（0~4）
  uint32_t totalCompleted = 0;      // 总完成番茄数
};

// 复位：回到初始待机界面
void pomodoroReset(PomodoroState& s);

// 空格：启动 / 暂停
void pomodoroToggle(PomodoroState& s, uint32_t now);

// S：跳过当前计时，直接进入下一阶段（不计入完成数）
void pomodoroSkip(PomodoroState& s, uint32_t now);

// 主动暂停（陀螺仪晃动触发）
void pomodoroPause(PomodoroState& s);

// 每帧调用，推进倒计时。倒计时归零时进入响铃状态（不自动切阶段）
void pomodoroTick(PomodoroState& s, uint32_t now);

// 响铃结束后调用，切换并自动启动下一阶段
void pomodoroFinishBeep(PomodoroState& s, uint32_t now);

// 当前阶段名称（学习中/短休息/长休息）
const char* pomodoroPhaseName(const PomodoroState& s);

// 下一阶段时长（毫秒）
uint32_t pomodoroPhaseDuration(Phase p);
