// ============================================================
// Mesh应急通讯控制台 - 主程序
// 硬件：M5Stack Cardputer-Adv (ESP32-S3) + Cap LoRa-1262
//
// 按键说明（fn = 键盘左下角 fn 键）：
//   常规按键        聊天界面输入文字
//   Enter           发送 / 确认 / 开始编辑设置项
//   fn + [ / ]      上 / 下（聊天滚动、列表选择、设置项选择）
//   fn + 1..5       聊天 / 设备 / 状态 / 航点 / 设置
//   fn + S          开始 / 停止 紧急求救广播
//   fn + L          立即发送本机定位
//   fn + P          保存当前坐标为航点
//   fn + A          取消设置编辑（编辑态）
//   晃动设备        快速发送本机位置
//   前后倾斜设备    上下滚动消息列表
// ============================================================

#include <M5Cardputer.h>
#include <math.h>

#include "config.h"
#include "gnss.h"
#include "mesh.h"
#include "radio.h"
#include "store.h"
#include "ui.h"

// ---------------- 陀螺仪体感状态 ----------------
static float    s_shakeAccum    = 0.0f;
static uint32_t s_lastImuMs     = 0;
static uint32_t s_lastShakeMs   = 0;
static float    s_neutralPitch  = 0.0f;
static bool     s_pitchCalibrated = false;
static uint32_t s_pitchCalibAt  = 0;
static uint32_t s_lastTiltMs    = 0;

// ---------------- 求救警报音 ----------------
static bool     s_alarmOn       = false;
static uint32_t s_alarmNext     = 0;

// ---------------- SD 日志同步 ----------------
static size_t   s_storedCount   = 0;

static void handleKey(uint32_t now);
static void updateMotion(uint32_t now);
static void updateSosAlarm();
static void updateSosOverlay();
static void storeSyncMessages();

// ---------------- 动作 ----------------
static void sendLocationNow() {
  const GnssData& g = gnssData();
  if (!g.valid) {
    uiToast("定位未锁定，无法发送");
    return;
  }
  meshSendLocation(g.lat, g.lon);
  uiToast("已发送当前位置");
}

static void saveWaypoint() {
  const GnssData& g = gnssData();
  if (!g.valid) {
    uiToast("定位未锁定，请到室外");
    return;
  }
  size_t n = storeWaypointCount();
  if (n >= WAYPOINT_MAX) {
    uiToast("航点已满");
    return;
  }
  char name[16];
  snprintf(name, sizeof(name), "W%u", (unsigned)(n + 1));
  if (storeAddWaypoint(name, g.lat, g.lon)) {
    char buf[44];
    snprintf(buf, sizeof(buf), "已存%s %.4f,%.4f", name, (double)g.lat,
             (double)g.lon);
    uiToast(buf);
  } else {
    uiToast("航点保存失败");
  }
}

static void sendSelectedWaypoint() {
  char name[40];
  float lat, lon;
  if (!storeWaypointAt((size_t)uiSel(), name, sizeof(name), &lat, &lon)) {
    uiToast("没有可选坐标");
    return;
  }
  meshSendLocation(lat, lon);
  uiToast("已发送所选坐标");
}

static void toggleSos() {
  if (meshSosActive()) {
    meshStopSos();
    M5Cardputer.Speaker.stop();
    uiToast("已停止求救广播");
  } else {
    meshStartSos();
    uiToast("开始求救广播");
  }
}

static void sendInput() {
  const char* text = uiInputText();
  if (text[0] == '\0') return;
  meshSendText(text);
  uiInputClear();
}

// ---------------- 键盘处理 ----------------
static void handleKey(uint32_t now) {
  if (!M5Cardputer.Keyboard.isChange()) return;
  if (!M5Cardputer.Keyboard.isPressed()) return;

  const auto& st = M5Cardputer.Keyboard.keysState();
  bool fn = st.fn;

  // 求救遮罩优先：Enter 确认消除警报；fn+S 可停止本地求救
  if (uiSosOverlayVisible()) {
    if (st.enter) {
      if (meshSosActive()) {
        uiToast("fn+S 停止求救");
      } else {
        meshAckSos();
        uiSosOverlayHide();
        M5Cardputer.Speaker.stop();
      }
    } else if (fn && !st.word.empty() && (st.word[0] == 's' || st.word[0] == 'S')) {
      toggleSos();
      M5Cardputer.Speaker.stop();
    }
    return;
  }

  // fn 组合快捷键
  if (fn) {
    char k = st.word.empty() ? '\0' : st.word[0];
    switch (k) {
      case '1': uiGoto(Screen::CHAT); break;
      case '2': uiGoto(Screen::DEVICES); break;
      case '3': uiGoto(Screen::STATUS); break;
      case '4': uiGoto(Screen::WAYPOINTS); break;
      case '5': uiGoto(Screen::SETTINGS); break;
      case '[':  // 上：聊天上翻 / 列表上移
        if (uiScreen() == Screen::CHAT) uiChatScrollBy(-15);
        else uiSelMove(-1);
        break;
      case ']':  // 下
        if (uiScreen() == Screen::CHAT) uiChatScrollBy(15);
        else uiSelMove(1);
        break;
      case 'b': case 'B':
        uiChatScrollToBottom();
        break;
      case 's': case 'S': toggleSos(); break;
      case 'l': case 'L': sendLocationNow(); break;
      case 'p': case 'P': saveWaypoint(); break;
      case 'a': case 'A':
        if (uiSettingEditing()) uiSettingCancelEdit();
        break;
      default: break;
    }
    return;
  }

  // Enter：随界面上下文处理
  if (st.enter) {
    switch (uiScreen()) {
      case Screen::CHAT:     sendInput(); break;
      case Screen::WAYPOINTS: sendSelectedWaypoint(); break;
      case Screen::SETTINGS:
        if (uiSettingEditing()) uiSettingCommitEdit();
        else uiSettingStartEdit();
        break;
      default: break;
    }
    return;
  }

  // 退格
  if (st.del) {
    uiInputBackspace();
    return;
  }

  // 常规字符输入
  for (char c : st.word) {
    if (uiScreen() == Screen::CHAT || uiSettingEditing()) {
      uiInputAppend(c);
    }
  }
}

// ---------------- 陀螺仪：晃动发送位置 + 倾斜滚动 ----------------
static float getPitchDeg() {
  float ax, ay, az;
  if (!M5.Imu.getAccel(&ax, &ay, &az)) return 0.0f;
  return atan2f(ay, az) * 180.0f / PI;
}

static void updateMotion(uint32_t now) {
  // ---- 晃动（角速度）----
  float gx, gy, gz;
  if (M5.Imu.getGyro(&gx, &gy, &gz)) {
    uint32_t dt = now - s_lastImuMs;
    if (dt > 100) dt = 100;
    s_lastImuMs = now;

    float mag = sqrtf(gx * gx + gy * gy + gz * gz);
    if (mag > SHAKE_GYRO_THRESHOLD) {
      s_shakeAccum += (float)dt;
    } else {
      s_shakeAccum -= (float)dt * 0.3f;
      if (s_shakeAccum < 0) s_shakeAccum = 0;
    }

    if (s_shakeAccum >= (float)SHAKE_TRIGGER_MS &&
        now - s_lastShakeMs >= SHAKE_COOLDOWN_MS) {
      s_shakeAccum = 0;
      s_lastShakeMs = now;
      if (uiScreen() == Screen::CHAT || uiScreen() == Screen::STATUS) {
        sendLocationNow();
      }
    }
  }

  // ---- 倾斜（加速度计俯仰角）----
  if (!s_pitchCalibrated) {
    if (s_pitchCalibAt == 0) {
      s_pitchCalibAt = now;
    } else if (now - s_pitchCalibAt > 1500) {
      s_neutralPitch = getPitchDeg();
      s_pitchCalibrated = true;
    }
    return;
  }

  float pitch = getPitchDeg();
  float delta = pitch - s_neutralPitch;

  if (fabsf(delta) > TILT_SCROLL_DEG && now - s_lastTiltMs >= TILT_SCROLL_COOLDOWN) {
    s_lastTiltMs = now;
    if (uiScreen() == Screen::CHAT) {
      uiChatScrollBy(delta > 0 ? 15 : -15);
    } else if (uiScreen() == Screen::DEVICES ||
               uiScreen() == Screen::WAYPOINTS ||
               uiScreen() == Screen::SETTINGS) {
      uiSelMove(delta > 0 ? 1 : -1);
    }
  }
}

// ---------------- 求救警报音（持续直到确认） ----------------
static void updateSosAlarm() {
  bool want = uiSosOverlayVisible();
  uint32_t now = millis();

  if (!want) {
    if (s_alarmOn) {
      M5Cardputer.Speaker.stop();
      s_alarmOn = false;
    }
    return;
  }

  if (now >= s_alarmNext) {
    s_alarmOn = !s_alarmOn;
    if (s_alarmOn) {
      M5Cardputer.Speaker.tone(SOS_ALARM_FREQ_HZ, 320);
      s_alarmNext = now + 320;
    } else {
      M5Cardputer.Speaker.stop();
      s_alarmNext = now + 160;
    }
  }
}

// ---------------- 求救遮罩跟随状态 ----------------
static void updateSosOverlay() {
  if (uiSosOverlayVisible()) {
    // 本地求救进行中：保持
    if (meshSosActive()) return;
    // 外部告警：确认（meshAckSos）或对方发送求救结束（SOS_END）后消失
    if (!meshSosAlertActive()) {
      uiSosOverlayHide();
      M5Cardputer.Speaker.stop();
    }
    return;
  }
  if (meshSosActive()) {
    uiSosOverlayShow(meshSelfId(), true);
  } else if (meshSosAlertActive()) {
    uiSosOverlayShow(meshSosAlertSource(), false);
  }
}

// ---------------- 新消息落 SD ----------------
static void storeSyncMessages() {
  size_t cnt = meshMessageCount();
  if (cnt == 0) {
    s_storedCount = 0;
    return;
  }
  if (cnt < s_storedCount) s_storedCount = 0;  // 缓冲回绕，简单对齐
  if (cnt > s_storedCount) {
    for (size_t i = s_storedCount; i < cnt; i++) {
      storeLogMessage(meshMessageAt(i));
    }
    s_storedCount = cnt;
  }
}

// ---------------- 启动 ----------------
void setup() {
  Serial.begin(115200);
  auto cfg = M5.config();
  cfg.internal_imu = true;  // BMI270
  cfg.internal_spk = true;  // 扬声器（Adv 为 ES8311 方案）
  M5Cardputer.begin(cfg, true);

  M5Cardputer.Display.setRotation(1);  // 横向 240x135
  M5Cardputer.Speaker.setVolume(160);

  uiInit();

  bool radioOk = radioInit();
  meshInit();        // 加载本机设置（名称/密钥/射频参数），并应用到射频
  storeBegin();      // SD 卡（与 LoRa 共用 SPI 总线）
  gnssInit();        // GNSS
  storeLoadHistory(MAX_MESSAGES);  // 恢复上次开机前的聊天记录

  uiToast(radioOk ? "系统就绪" : "射频初始化失败");
  if (storeAvailable()) uiToast("已载入历史记录");

  Serial.printf("[mesh] 就绪 radio=%s(code %u) sd=%s canvas=%s\n",
                radioOk ? "OK" : "FAIL", (unsigned)radioGetStatus().lastState,
                storeAvailable() ? "OK" : "无SD卡",
                uiCanvasActive() ? "OK" : "FAIL");
}

// ---------------- 主循环 ----------------
void loop() {
  uint32_t now = millis();
  M5Cardputer.update();

  handleKey(now);

  // 无线收发常驻（切界面不中断）
  radioPoll();
  if (radioHasFrame()) {
    uint8_t buf[RADIO_MAX_FRAME];
    size_t len;
    float rssi, snr;
    if (radioReadFrame(buf, &len, &rssi, &snr)) {
      meshOnFrame(buf, len, rssi, snr, now);
    }
  }
  meshTick(now);

  gnssUpdate();
  updateMotion(now);
  updateSosOverlay();
  updateSosAlarm();
  storeSyncMessages();

  uiRender();

  static uint32_t s_lastHeartbeat = 0;
  if (now - s_lastHeartbeat >= 10000) {
    s_lastHeartbeat = now;
    const GnssData& g = gnssData();
    if (g.valid) {
      Serial.printf("[mesh] alive %lus dev=%d gps=OK %.5f,%.5f sat=%d\n",
                    (unsigned long)(now / 1000), (int)meshDeviceCount(),
                    (double)g.lat, (double)g.lon, g.sats);
    } else {
      Serial.printf("[mesh] alive %lus dev=%d gps=--\n",
                    (unsigned long)(now / 1000), (int)meshDeviceCount());
    }
    Serial.printf("  rf=%.1fMHz/SF%d/sw=0x%02X key=%s\n", (double)meshGetFreq(),
                  meshGetSf(), meshGetSyncWord(), meshGetKey()[0] ? "on" : "off");
  }
}
