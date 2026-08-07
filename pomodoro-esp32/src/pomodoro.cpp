#include "pomodoro.h"

uint32_t pomodoroPhaseDuration(Phase p) {
  switch (p) {
    case Phase::WORK: return WORK_MS;
    case Phase::SHORT_BREAK: return SHORT_BREAK_MS;
    case Phase::LONG_BREAK: return LONG_BREAK_MS;
    default: return WORK_MS;
  }
}

const char* pomodoroPhaseName(const PomodoroState& s) {
  switch (s.phase) {
    case Phase::WORK: return "学习中";
    case Phase::SHORT_BREAK: return "短休息";
    case Phase::LONG_BREAK: return "长休息";
    default: return "学习中";
  }
}

void pomodoroReset(PomodoroState& s) {
  s.phase = Phase::IDLE;
  s.running = false;
  s.beeping = false;
  s.remainingMs = WORK_MS;
  s.completedInCycle = 0;
  s.totalCompleted = 0;
}

void pomodoroPause(PomodoroState& s) {
  s.running = false;
  s.lastTickMs = 0;
}

void pomodoroToggle(PomodoroState& s, uint32_t now) {
  if (s.beeping) return;  // 响铃期间忽略按键

  if (s.phase == Phase::IDLE) {
    // 待机 → 开始第一个番茄
    s.phase = Phase::WORK;
    s.remainingMs = WORK_MS;
    s.running = true;
    s.lastTickMs = now;
    return;
  }

  s.running = !s.running;
  if (s.running) {
    s.lastTickMs = now;
  }
}

// 进入下一阶段（running 由调用方决定）
static void enterPhase(PomodoroState& s, Phase p, uint32_t now) {
  s.phase = p;
  s.remainingMs = pomodoroPhaseDuration(p);
  s.running = true;
  s.lastTickMs = now;
}

void pomodoroSkip(PomodoroState& s, uint32_t now) {
  s.beeping = false;
  s.beepUntilMs = 0;

  switch (s.phase) {
    case Phase::IDLE:
      enterPhase(s, Phase::WORK, now);
      break;
    case Phase::WORK:
      // 跳过不计数，直接按本周期进度进入对应休息
      if (s.completedInCycle >= CYCLE_LENGTH) {
        enterPhase(s, Phase::LONG_BREAK, now);
      } else {
        enterPhase(s, Phase::SHORT_BREAK, now);
      }
      break;
    case Phase::SHORT_BREAK:
      enterPhase(s, Phase::WORK, now);
      break;
    case Phase::LONG_BREAK:
      s.completedInCycle = 0;
      enterPhase(s, Phase::WORK, now);
      break;
  }
}

void pomodoroTick(PomodoroState& s, uint32_t now) {
  if (s.beeping) return;

  if (s.phase == Phase::IDLE || !s.running) {
    s.lastTickMs = now;
    return;
  }

  uint32_t elapsed = now - s.lastTickMs;
  s.lastTickMs = now;

  if (elapsed >= s.remainingMs) {
    // 倒计时归零：进入响铃状态，等待响铃结束再切阶段
    s.remainingMs = 0;
    s.running = false;
    s.beeping = true;
    s.beepUntilMs = now + BEEP_MS;
    return;
  }
  s.remainingMs -= elapsed;
}

void pomodoroFinishBeep(PomodoroState& s, uint32_t now) {
  if (!s.beeping) return;
  s.beeping = false;
  s.beepUntilMs = 0;

  switch (s.phase) {
    case Phase::WORK:
      // 完成一个番茄
      s.completedInCycle++;
      s.totalCompleted++;
      if (s.completedInCycle >= CYCLE_LENGTH) {
        enterPhase(s, Phase::LONG_BREAK, now);
      } else {
        enterPhase(s, Phase::SHORT_BREAK, now);
      }
      break;
    case Phase::SHORT_BREAK:
    case Phase::LONG_BREAK:
      if (s.phase == Phase::LONG_BREAK) s.completedInCycle = 0;
      enterPhase(s, Phase::WORK, now);
      break;
    default:
      break;
  }
}
