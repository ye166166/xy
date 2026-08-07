#include <M5Cardputer.h>
#include <math.h>

#include "config.h"
#include "pomodoro.h"

// ============================================================
// 番茄时钟主程序
// 硬件层使用 M5Unified / M5Cardputer 库（不改动底层驱动），
// 业务逻辑在 pomodoro.{h,cpp} 中与硬件完全解耦。
//
// 按键:
//   空格  启动 / 暂停
//   S     跳过当前阶段（不计入完成数）
//   R     重置整套番茄时钟
// 附加: 连续大幅晃动设备 3 秒 -> 暂停计时（BMI270 陀螺仪）
// ============================================================

static PomodoroState state;

// ---- 渲染快照（仅内容变化时重绘） ----
struct Snapshot {
  Phase    phase;
  bool     running;
  bool     beeping;
  uint32_t remSec;
  uint8_t  completedInCycle;
};
static Snapshot lastSnapshot = {};
static bool     hasSnapshot  = false;

// ---- 扬声器状态 ----
static bool beepPlaying = false;

// ---- 晃动检测状态 ----
static float    shakeAccum       = 0;
static uint32_t lastImuMs        = 0;
static uint32_t lastShakeTrigger = 0;
static bool     imuWarned        = false;

static void render(void);

static void handleKey(char key, uint32_t now) {
  if (state.beeping) {
    // 响铃期间按键：提前结束提示音并切到下一阶段
    M5Cardputer.Speaker.stop();
    beepPlaying        = false;
    state.beeping      = false;
    state.beepUntilMs  = 0;
    pomodoroFinishBeep(state, now);
  }

  switch (key) {
    case ' ':
      pomodoroToggle(state, now);
      break;
    case 's':
    case 'S':
      pomodoroSkip(state, now);
      break;
    case 'r':
    case 'R':
      pomodoroReset(state);
      break;
  }
}

static void updateShake(uint32_t now) {
  float gx, gy, gz;
  if (!M5.Imu.getGyro(&gx, &gy, &gz)) {
    if (!imuWarned) {
      Serial.println("[imu] BMI270 不可用，晃动暂停功能已禁用");
      imuWarned = true;
    }
    return;
  }

  uint32_t dt = now - lastImuMs;
  if (dt > 100) dt = 100;  // 防止首次读取/卡顿导致突增
  lastImuMs = now;

  float mag = sqrtf(gx * gx + gy * gy + gz * gz);  // 合成角速度(deg/s)
  if (mag > SHAKE_GYRO_THRESHOLD) {
    shakeAccum += dt;
  } else {
    shakeAccum -= dt * SHAKE_DECAY;
    if (shakeAccum < 0) shakeAccum = 0;
  }

  if (shakeAccum >= SHAKE_TRIGGER_MS &&
      now - lastShakeTrigger >= SHAKE_COOLDOWN_MS) {
    shakeAccum        = 0;
    lastShakeTrigger  = now;
    if (state.running) {
      pomodoroPause(state);
      Serial.println("[shake] 检测到连续晃动 3s -> 暂停");
    }
  }
}

static bool snapshotChanged(void) {
  Snapshot s = {
      state.phase,
      state.running,
      state.beeping,
      state.remainingMs / 1000,
      state.completedInCycle,
  };
  bool changed = !hasSnapshot ||
                 s.phase != lastSnapshot.phase ||
                 s.running != lastSnapshot.running ||
                 s.beeping != lastSnapshot.beeping ||
                 s.remSec != lastSnapshot.remSec ||
                 s.completedInCycle != lastSnapshot.completedInCycle;
  lastSnapshot = s;
  hasSnapshot  = true;
  return changed;
}

static void render(void) {
  auto& d = M5Cardputer.Display;
  d.fillScreen(TFT_WHITE);
  d.setTextColor(TFT_BLACK, TFT_WHITE);
  int cx = d.width() / 2;

  // 阶段名称
  d.setFont(&fonts::efontCN_24);
  d.setTextDatum(top_center);
  d.drawString(pomodoroPhaseName(state), cx, 2);

  // 剩余时分秒（放大显示）
  d.setFont(&fonts::FreeSansBold24pt7b);
  char tbuf[16];
  uint32_t totalSec = state.remainingMs / 1000;
  if (totalSec >= 3600) {
    snprintf(tbuf, sizeof(tbuf), "%lu:%02lu:%02lu", totalSec / 3600,
             (totalSec % 3600) / 60, totalSec % 60);
  } else {
    snprintf(tbuf, sizeof(tbuf), "%02lu:%02lu", totalSec / 60, totalSec % 60);
  }
  d.drawString(tbuf, cx, 32);

  // 状态提示
  d.setFont(&fonts::efontCN_16);
  const char* status;
  if (state.beeping) {
    status = "时间到!";
  } else if (state.phase == Phase::IDLE) {
    status = "按空格开始";
  } else if (state.running) {
    status = "运行中";
  } else {
    status = "已暂停";
  }
  d.drawString(status, cx, 78);

  // 已完成番茄数量
  char cbuf[32];
  snprintf(cbuf, sizeof(cbuf), "已完成 %d / %d", state.completedInCycle,
           CYCLE_LENGTH);
  d.drawString(cbuf, cx, 96);

  // 本周期进度圆点
  for (int i = 0; i < CYCLE_LENGTH; i++) {
    int x = cx - (CYCLE_LENGTH - 1) * 12 + i * 24;
    if (i < state.completedInCycle) {
      d.fillCircle(x, 120, 5, TFT_BLACK);
    } else {
      d.drawCircle(x, 120, 5, TFT_BLACK);
    }
  }
}

void setup() {
  auto cfg = M5.config();
  cfg.internal_imu = true;  // BMI270
  cfg.internal_spk = true;  // 内置扬声器
  M5Cardputer.begin(cfg, true);

  M5Cardputer.Display.setRotation(1);  // 横向 240x135
  M5Cardputer.Speaker.setVolume(BEEP_VOLUME);

  pomodoroReset(state);
  render();

  Serial.println("[pomodoro] 就绪: 空格=启动/暂停  S=跳过  R=重置  晃动3s=暂停");
}

void loop() {
  uint32_t now = millis();
  M5Cardputer.update();

  // 实体键盘
  if (M5Cardputer.Keyboard.isChange() &&
      M5Cardputer.Keyboard.isPressed()) {
    const auto& word = M5Cardputer.Keyboard.keysState().word;
    for (char k : word) {
      handleKey(k, now);
    }
  }

  // 状态机计时
  pomodoroTick(state, now);

  // 扬声器提示音（持续 2 秒）
  if (state.beeping && !beepPlaying) {
    M5Cardputer.Speaker.tone(BEEP_FREQ_HZ, BEEP_MS);
    beepPlaying = true;
  }
  if (!state.beeping && beepPlaying) {
    beepPlaying = false;
  }
  if (state.beeping && now >= state.beepUntilMs) {
    M5Cardputer.Speaker.stop();
    beepPlaying = false;
    pomodoroFinishBeep(state, now);
  }

  // 陀螺仪晃动检测
  updateShake(now);

  // 重绘
  if (snapshotChanged()) {
    render();
  }
}
