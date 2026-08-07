// ============================================================
// ui.cpp - 深色科技风界面
// ============================================================

#include "ui.h"

#include <M5Cardputer.h>
#include <cstring>

#include "config.h"
#include "gnss.h"
#include "mesh.h"
#include "radio.h"
#include "store.h"

// ---------------- 配色（深色科技风） ----------------
static constexpr uint16_t C_BG     = 0x0000;
static constexpr uint16_t C_PANEL  = 0x0821;
static constexpr uint16_t C_PANEL2 = 0x1084;
static constexpr uint16_t C_LINE   = 0x2965;
static constexpr uint16_t C_TEXT   = 0xDEDB;
static constexpr uint16_t C_DIM    = 0x6B6D;
static constexpr uint16_t C_ACCENT = 0x07FF;   // 青
static constexpr uint16_t C_GREEN  = 0x07E0;
static constexpr uint16_t C_YELLOW = 0xFFE0;
static constexpr uint16_t C_ORANGE = 0xFD20;
static constexpr uint16_t C_RED    = 0xF000;
static constexpr uint16_t C_WHITE  = 0xFFFF;

// ---------------- 布局 ----------------
static constexpr int SCREEN_W = 240;
static constexpr int SCREEN_H = 135;
static constexpr int HEADER_H = 14;
static constexpr int INPUT_H  = 18;
static constexpr int VIEW_H   = SCREEN_H - HEADER_H - INPUT_H;

static constexpr int MSG_MAX_INPUT = 96;

// ---------------- 字体 ----------------
static const lgfx::U8g2font* F_MINI  = &fonts::efontCN_10;
static const lgfx::U8g2font* F_TEXT  = &fonts::efontCN_12;
static const lgfx::U8g2font* F_MED   = &fonts::efontCN_14;
static const lgfx::U8g2font* F_BIG   = &fonts::efontCN_16;

// ---------------- 双缓冲（消除整屏重绘闪烁） ----------------
static m5gfx::M5Canvas* s_canvas = nullptr;

static lgfx::LGFXBase& D() {
  if (s_canvas) return *s_canvas;
  return M5Cardputer.Display;
}

// ---------------- UI 状态 ----------------
static bool     s_dirty        = true;
static Screen   s_screen       = Screen::CHAT;

static char     s_input[MSG_MAX_INPUT + 1] = {0};
static size_t   s_inputLen = 0;

static int      s_scrollPx   = 0;
static bool     s_atBottom   = true;
static size_t   s_lastMsgCnt = 0;

static int      s_sel        = 0;

static bool     s_editing    = false;

static bool     s_sosOvVisible = false;
static uint32_t s_sosOvSource  = 0;
static bool     s_sosOvLocal   = false;

static char     s_toast[64]  = {0};
static uint32_t s_toastUntil = 0;

// ---------------- 工具 ----------------
static void hexId(uint32_t id, char* out) {
  snprintf(out, 9, "%08X", (unsigned)id);
}

static void upTime(uint32_t ms, char* out) {
  uint32_t s = ms / 1000;
  snprintf(out, 12, "%02lu:%02lu", (unsigned long)(s / 60) % 100, (unsigned long)(s % 60));
}

// 文本按像素宽度换行（使用 M5GFX textLength 精确测量）
static void wrapText(const char* text, int maxW,
                     char out[][MSG_MAX_LEN + 1], uint8_t maxLines, uint8_t* nOut) {
  int len = (int)strlen(text);
  int pos = 0;
  int n = 0;
  while (pos < len && n < maxLines) {
    int fit = D().textLength(text + pos, maxW);
    if (fit <= 0) fit = 1;  // 至少推进一个字符，防止死循环
    int end = pos + fit;
    if (end > len) end = len;
    // 若还有剩余文本，尝试在空格处回退断行
    if (end < len) {
      int sp = end;
      while (sp > pos && text[sp] != ' ') sp--;
      if (sp > pos) end = sp;
    }
    int ll = end - pos;
    if (ll > MSG_MAX_LEN) ll = MSG_MAX_LEN;
    memcpy(out[n], text + pos, (size_t)ll);
    out[n][ll] = '\0';
    n++;
    pos = end;
    while (pos < len && text[pos] == ' ') pos++;
  }
  if (n == 0) { out[0][0] = '\0'; n = 1; }
  *nOut = (uint8_t)n;
}

static void drawHeader(const char* title) {
  D().fillRect(0, 0, SCREEN_W, HEADER_H, C_PANEL);
  D().drawLine(0, HEADER_H - 1, SCREEN_W, HEADER_H - 1, C_LINE);

  D().setFont(F_MINI);
  D().setTextSize(1);
  D().setTextDatum(top_left);
  D().setTextColor(C_ACCENT, C_PANEL);
  char idbuf[9];
  hexId(meshSelfId(), idbuf);
  D().drawString(idbuf, 4, 2);

  D().setTextColor(C_DIM, C_PANEL);
  D().drawString(meshSelfName(), 66, 2);

  // 右侧：定位 + 射频状态
  D().setTextColor(C_DIM, C_PANEL);
  D().drawString("GPS:", 168, 2);
  const GnssData& g = gnssData();
  if (g.valid) {
    D().setTextColor(C_GREEN, C_PANEL);
    D().drawString("OK", 196, 2);
  } else {
    D().setTextColor(C_RED, C_PANEL);
    D().drawString("--", 196, 2);
  }
  D().setTextColor(C_DIM, C_PANEL);
  D().drawString("RF:", 214, 2);
  const RadioStatus& rs = radioGetStatus();
  D().setTextColor(rs.radioOk ? C_GREEN : C_RED, C_PANEL);
  D().drawString(rs.radioOk ? "OK" : "!!", 232, 2);
}

static void drawInputLine(const char* prompt) {
  int y = SCREEN_H - INPUT_H;
  D().fillRect(0, y, SCREEN_W, INPUT_H, C_PANEL);
  D().drawLine(0, y, SCREEN_W, y, C_LINE);

  D().setFont(F_TEXT);
  D().setTextDatum(top_left);
  D().setTextColor(C_ACCENT, C_PANEL);
  D().drawString(prompt, 4, y + 3);

  D().setTextColor(C_WHITE, C_PANEL);
  D().drawString(s_input, 20, y + 3);

  // 静态光标（双缓冲下无需闪烁重绘）
  int cx = 20 + D().textWidth(s_input);
  if (cx < SCREEN_W - 8) {
    D().fillRect(cx, y + 4, 6, 10, C_WHITE);
  }
}

static void drawBottomHint(const char* text) {
  int y = SCREEN_H - INPUT_H;
  D().fillRect(0, y, SCREEN_W, INPUT_H, C_PANEL);
  D().drawLine(0, y, SCREEN_W, y, C_LINE);
  D().setFont(F_MINI);
  D().setTextDatum(top_left);
  D().setTextColor(C_DIM, C_PANEL);
  D().drawString(text, 4, y + 4);
}

// ---------------- 聊天界面 ----------------
struct ChatBlock {
  char meta[48];
  char lines[4][MSG_MAX_LEN + 1];
  uint8_t nLines;
  uint16_t metaColor;
  bool incoming;
};

// 计算第 i 条消息的布局（写入 block）
static void layoutMessage(size_t i, ChatBlock& b) {
  const int maxW = SCREEN_W - 8;
  const ChatMsg& m = meshMessageAt(i);

  D().setFont(F_TEXT);  // 确保 textLength 使用正确的字体与字号
  D().setTextSize(1);

  char tbuf[12];
  upTime(m.rxMs, tbuf);
  if (m.incoming) {
    char rssi[12];
    snprintf(rssi, sizeof(rssi), "%ddBm", (int)m.rssi);
    if (m.name[0]) {
      snprintf(b.meta, sizeof(b.meta), "[%s] %s %s", m.name, tbuf, rssi);
    } else {
      char id[9];
      hexId(m.srcId, id);
      snprintf(b.meta, sizeof(b.meta), "[%s] %s %s", id, tbuf, rssi);
    }
    b.metaColor = C_DIM;
  } else {
    snprintf(b.meta, sizeof(b.meta), "[我] %s", tbuf);
    b.metaColor = C_ACCENT;
  }

  char content[MSG_MAX_LEN + 8];
  if (m.type == MSG_LOCATION) {
    snprintf(content, sizeof(content), "坐标 %.5f, %.5f", (double)m.lat, (double)m.lon);
  } else if (m.type == MSG_SOS) {
    snprintf(content, sizeof(content), "SOS %s", m.text[0] ? m.text : "求救");
  } else {
    strncpy(content, m.encrypted ? "<加密消息>" : (m.text[0] ? m.text : "（空）"),
            sizeof(content) - 1);
    content[sizeof(content) - 1] = '\0';
  }

  uint8_t nl = 0;
  wrapText(content, maxW, b.lines, 4, &nl);
  b.nLines = nl;
  b.incoming = m.incoming;
}

static void renderChat() {
  D().fillScreen(C_BG);
  drawHeader("聊天");

  // 检测新消息 -> 若在底部则跟随
  size_t cnt = meshMessageCount();
  if (cnt != s_lastMsgCnt) {
    if (s_atBottom) s_scrollPx = 0x7FFFFFFF;  // 标记，稍后钳制到底
    s_lastMsgCnt = cnt;
  }

  const int META_H = 13;
  const int LINE_H = 15;

  // 第一遍：求总高度
  int totalH = 0;
  ChatBlock scratch;
  for (size_t i = 0; i < cnt; i++) {
    layoutMessage(i, scratch);
    totalH += META_H + scratch.nLines * LINE_H;
  }

  // 视图窗口
  int vpTop = HEADER_H;
  int vpH   = VIEW_H;
  int maxScroll = totalH > vpH ? (totalH - vpH) : 0;
  if (s_scrollPx > 0x100000) s_scrollPx = maxScroll;  // 标记到顶
  if (s_scrollPx > maxScroll) s_scrollPx = maxScroll;
  if (s_scrollPx < 0) s_scrollPx = 0;
  s_atBottom = (s_scrollPx >= maxScroll);

  // 第二遍：只布局并绘制可见区间
  int y = HEADER_H - s_scrollPx;
  for (size_t i = 0; i < cnt; i++) {
    layoutMessage(i, scratch);
    int bh = META_H + scratch.nLines * LINE_H;
    if (y + bh < vpTop) { y += bh; continue; }
    if (y > vpTop + vpH) break;

    D().setFont(F_MINI);
    D().setTextDatum(top_left);
    D().setTextColor(scratch.metaColor, C_BG);
    D().drawString(scratch.meta, 4, y);
    y += META_H;

    D().setFont(F_TEXT);
    for (uint8_t k = 0; k < scratch.nLines; k++) {
      D().setTextColor(scratch.incoming ? C_TEXT : C_ACCENT, C_BG);
      D().drawString(scratch.lines[k], 4, y);
      y += LINE_H;
    }
  }

  drawInputLine(">");
}

// ---------------- 设备列表界面 ----------------
static void renderDevices() {
  D().fillScreen(C_BG);
  drawHeader("设备");

  const int rowH = 34;
  size_t n = meshDeviceCount();

  int vpTop = HEADER_H;
  int vpH   = VIEW_H;
  int visRows = vpH / rowH;
  if (visRows < 1) visRows = 1;

  // 选择项跟随
  int start = 0;
  if (s_sel < start) start = s_sel;
  if (s_sel >= (int)n && n > 0) s_sel = (int)n - 1;
  if (n > 0 && s_sel >= start + visRows) start = s_sel - visRows + 1;
  if ((int)n <= visRows) start = 0;

  int y = vpTop;
  for (size_t i = 0; i < n; i++) {
    if ((int)i < start) continue;
    if ((int)i >= start + visRows) break;
    const DeviceEntry& e = meshDevices()[i];

    if ((int)i == s_sel) D().fillRect(0, y, SCREEN_W, rowH, C_PANEL2);

    // 状态点
    uint16_t dot = C_DIM;
    if (e.sosSeen) dot = C_RED;
    else if (e.online) dot = C_GREEN;
    D().fillCircle(8, y + 9, 3, dot);

    // 名称 / ID
    D().setFont(F_TEXT);
    D().setTextDatum(top_left);
    char label[NAME_MAX_LEN + 12];
    if (e.name[0]) {
      snprintf(label, sizeof(label), "%s", e.name);
    } else {
      char id[9];
      hexId(e.id, id);
      snprintf(label, sizeof(label), "%s", id);
    }
    D().setTextColor(C_TEXT, 0);
    D().drawString(label, 16, y + 1);

    // RSSI
    char rssi[16];
    snprintf(rssi, sizeof(rssi), "%ddBm", (int)e.lastRssi);
    D().setFont(F_MINI);
    D().setTextColor(C_DIM, 0);
    D().setTextDatum(top_right);
    D().drawString(rssi, SCREEN_W - 4, y + 3);

    // 状态文本 + 定位
    D().setTextDatum(top_left);
    if (e.sosSeen) {
      D().setTextColor(C_RED, 0);
      D().drawString("求救中", 16, y + 16);
    } else if (e.online) {
      D().setTextColor(C_GREEN, 0);
      D().drawString("在线", 16, y + 16);
    } else {
      D().setTextColor(C_DIM, 0);
      D().drawString("离线", 16, y + 16);
    }

    if (e.hasLocation) {
      char loc[28];
      snprintf(loc, sizeof(loc), "%.4f,%.4f", (double)e.lat, (double)e.lon);
      D().setTextColor(C_DIM, 0);
      D().drawString(loc, 54, y + 16);
    }

    // 信号强度波形（历史）
    if (e.rssiHistLen > 0) {
      int bx = 150, by = y + 18;
      for (uint8_t k = 0; k < e.rssiHistLen && k < 16; k++) {
        int r = e.rssiHist[k];
        int h = 10;
        if (r < -120) h = 2;
        else if (r < -100) h = 4;
        else if (r < -90) h = 6;
        else if (r < -75) h = 8;
        else h = 10;
        uint16_t c = (r >= -80) ? C_GREEN : ((r >= -100) ? C_YELLOW : C_ORANGE);
        D().fillRect(bx + k * 4, by - h + 4, 3, h, c);
      }
    }
    y += rowH;
  }

  if (n == 0) {
    D().setFont(F_TEXT);
    D().setTextDatum(top_center);
    D().setTextColor(C_DIM, C_BG);
    D().drawString("尚未发现组网设备", SCREEN_W / 2, vpTop + 30);
  }

  char hint[48];
  snprintf(hint, sizeof(hint), "设备 %d 台 | [fn+2]设备 [fn+1]聊天", (int)n);
  drawBottomHint(hint);
}

// ---------------- 本机状态界面 ----------------
static void renderStatus() {
  D().fillScreen(C_BG);
  drawHeader("状态");

  D().setFont(F_TEXT);
  D().setTextDatum(top_left);
  int y = HEADER_H + 2;

  auto line = [&](const char* k, const char* v, uint16_t vc) {
    D().setTextColor(C_DIM, C_BG);
    D().drawString(k, 4, y);
    D().setTextColor(vc, C_BG);
    D().drawString(v, 86, y);
    y += 13;
  };

  char buf[64];
  char id[9];
  hexId(meshSelfId(), id);
  line("设备编号", id, C_ACCENT);
  line("设备名称", meshSelfName(), C_ACCENT);

  // 互通三参数：频段 / 信道 / 通讯密钥
  snprintf(buf, sizeof(buf), "%.1fMHz SF%d %ddBm", (double)meshGetFreq(),
           meshGetSf(), (int)meshGetPower());
  line("频段", buf, C_YELLOW);
  snprintf(buf, sizeof(buf), "0x%02X", meshGetSyncWord());
  line("信道", buf, C_YELLOW);
  line("密钥", meshGetKey()[0] ? "已启用" : "未设置", meshGetKey()[0] ? C_YELLOW : C_RED);

  const RadioStatus& rs = radioGetStatus();
  line("射频", rs.radioOk ? "正常" : "失败", rs.radioOk ? C_GREEN : C_RED);

  const GnssData& g = gnssData();
  if (g.valid) {
    snprintf(buf, sizeof(buf), "%.5f, %.5f SAT:%d", (double)g.lat, (double)g.lon, g.sats);
    line("定位", buf, C_GREEN);
  } else {
    line("定位", "未锁定", C_YELLOW);
  }

  line("SD卡", storeAvailable() ? "正常" : "不可用", storeAvailable() ? C_GREEN : C_RED);
  snprintf(buf, sizeof(buf), "%d 台", (int)meshDeviceCount());
  line("在线设备", buf, C_TEXT);

  drawBottomHint("互通硬规则: 频段/信道/密钥 必须一致");
}

// ---------------- 航点界面 ----------------
static void renderWaypoints() {
  D().fillScreen(C_BG);
  drawHeader("航点");

  size_t n = storeWaypointCount();
  const int rowH = 22;
  int vpTop = HEADER_H;
  int vpH = VIEW_H;
  int visRows = vpH / rowH;
  if (visRows < 1) visRows = 1;

  if (n == 0) {
    D().setFont(F_TEXT);
    D().setTextDatum(top_center);
    D().setTextColor(C_DIM, C_BG);
    D().drawString("暂无坐标记录", SCREEN_W / 2, vpTop + 30);
    D().setFont(F_MINI);
    D().drawString("在有定位时按 fn+P 保存当前位置", SCREEN_W / 2, vpTop + 52);
  } else {
    if (s_sel >= (int)n) s_sel = (int)n - 1;
    int start = 0;
    if (s_sel >= start + visRows) start = s_sel - visRows + 1;

    int y = vpTop;
    for (size_t i = (size_t)start; i < n && (int)i < start + visRows; i++) {
      char name[40];
      float lat, lon;
      if (!storeWaypointAt(i, name, sizeof(name), &lat, &lon)) continue;

      if ((int)i == s_sel) D().fillRect(0, y, SCREEN_W, rowH, C_PANEL2);

      D().setFont(F_TEXT);
      D().setTextDatum(top_left);
      D().setTextColor(C_ACCENT, 0);
      D().drawString(name, 6, y + 1);

      char loc[28];
      snprintf(loc, sizeof(loc), "%.5f, %.5f", (double)lat, (double)lon);
      D().setFont(F_MINI);
      D().setTextColor(C_DIM, 0);
      D().drawString(loc, 6, y + 13);

      D().setTextDatum(top_right);
      D().setTextColor(C_DIM, 0);
      D().drawString("[Enter]发送", SCREEN_W - 4, y + 1);
      D().setTextDatum(top_left);
      y += rowH;
    }
  }

  drawBottomHint("fn+P 保存当前位置 | [Enter]发送所选坐标");
}

// ---------------- 设置界面 ----------------
static const char* SETTING_KEYS[] = {"设备名称", "通讯密钥", "频率(MHz)", "发射功率(dBm)", "扩频因子(SF)", "信道(同步字)"};
static const int SETTING_COUNT = 6;

static const char* settingValue(int idx) {
  static char v[48];
  switch (idx) {
    case 0: return meshSelfName();
    case 1: return meshGetKey()[0] ? meshGetKey() : "（未设置）";
    case 2: snprintf(v, sizeof(v), "%.1f", (double)meshGetFreq()); return v;
    case 3: snprintf(v, sizeof(v), "%d", (int)meshGetPower()); return v;
    case 4: snprintf(v, sizeof(v), "SF%d", meshGetSf()); return v;
    case 5: snprintf(v, sizeof(v), "0x%02X", meshGetSyncWord()); return v;
    default: return "";
  }
}

static void renderSettings() {
  D().fillScreen(C_BG);
  drawHeader("设置");

  const int rowH = 22;
  int y = HEADER_H + 2;
  int vpH = VIEW_H;
  int visRows = vpH / rowH;
  if (visRows < 1) visRows = 1;
  int start = 0;
  if (s_sel < 0) s_sel = 0;
  if (s_sel >= SETTING_COUNT) s_sel = SETTING_COUNT - 1;
  if (s_sel >= start + visRows) start = s_sel - visRows + 1;

  for (int i = start; i < SETTING_COUNT && i < start + visRows; i++) {
    if (i == s_sel) D().fillRect(0, y - 1, SCREEN_W, rowH, C_PANEL2);
    D().setFont(F_TEXT);
    D().setTextDatum(top_left);
    D().setTextColor(C_TEXT, 0);
    D().drawString(SETTING_KEYS[i], 6, y + 1);

    D().setTextColor(i == s_sel ? C_ACCENT : C_DIM, 0);
    D().setTextDatum(top_right);
    D().drawString(settingValue(i), SCREEN_W - 4, y + 1);
    D().setTextDatum(top_left);
    y += rowH;
  }

  if (s_editing) {
    drawInputLine(">");
    D().setFont(F_MINI);
    D().setTextDatum(top_left);
    D().setTextColor(C_YELLOW, C_BG);
    D().drawString("[Enter]保存 [fn+1]取消", 4, SCREEN_H - INPUT_H - 13);
  } else {
    drawBottomHint("[Enter]编辑 上/下选择");
  }
}

// ---------------- SOS 遮罩 ----------------
static void renderSosOverlay() {
  D().fillRect(0, 0, SCREEN_W, SCREEN_H, C_RED);
  D().setFont(F_BIG);
  D().setTextDatum(top_center);
  D().setTextColor(C_WHITE, C_RED);
  D().drawString("紧急求救", SCREEN_W / 2, 8);

  D().setFont(F_TEXT);
  D().setTextDatum(top_center);
  if (s_sosOvLocal) {
    D().drawString("正在循环广播求救信号", SCREEN_W / 2, 40);
    D().setFont(F_MINI);
    D().drawString("含本机实时定位", SCREEN_W / 2, 58);
  } else {
    char buf[48];
    if (meshFindDeviceName(s_sosOvSource, buf, sizeof(buf))) {
      char tmp[64];
      snprintf(tmp, sizeof(tmp), "来自设备 %s", buf);
      D().drawString(tmp, SCREEN_W / 2, 40);
    }
    D().setFont(F_MINI);
    D().drawString("对方正在求救，请及时响应", SCREEN_W / 2, 58);
  }

  D().setFont(F_BIG);
  D().setTextDatum(bottom_center);
  D().setTextColor(C_YELLOW, C_RED);
  D().drawString("按 Enter 确认消除警报", SCREEN_W / 2, SCREEN_H - 6);
}

// ---------------- 公共接口 ----------------
void uiInit() {
  static m5gfx::M5Canvas canvas(&M5Cardputer.Display);  // 延迟构造，避免静态初始化顺序问题
  D().setRotation(1);
  D().setTextColor(C_TEXT, C_BG);
  if (canvas.createSprite(SCREEN_W, SCREEN_H)) {
    s_canvas = &canvas;
  }
  s_dirty = true;
}

void uiForceRedraw() { s_dirty = true; }

bool uiCanvasActive() { return s_canvas != nullptr; }

// ---------------- 可见数据变化检测 ----------------
// 仅在数据真正变化时重绘，避免无意义的整屏刷新（闪烁的根源）。
static uint32_t s_lastDataCheckMs = 0;
static size_t   s_chatMsgCount    = 0;
static uint32_t s_chatLastRxMs    = 0;
static uint32_t s_chatGpsState    = 0;
static uint32_t s_devHash         = 0;
static size_t   s_devCount        = 0;
static uint32_t s_statusHash      = 0;
static size_t   s_waypointCount   = 0;

static bool uiDataChanged() {
  uint32_t now = millis();
  if (now - s_lastDataCheckMs < 300) return false;
  s_lastDataCheckMs = now;

  // toast 到期需重绘一次以清除
  if (s_toast[0] != '\0' && now >= s_toastUntil) return true;

  switch (s_screen) {
    case Screen::CHAT: {
      size_t cnt = meshMessageCount();
      uint32_t lastRx = (cnt > 0) ? meshMessageAt(cnt - 1).rxMs : 0;
      const GnssData& g = gnssData();
      uint32_t gpsState = (g.valid ? 0x10000 : 0) | (uint32_t)g.sats;
      bool changed = (cnt != s_chatMsgCount) || (lastRx != s_chatLastRxMs) ||
                     (gpsState != s_chatGpsState);
      s_chatMsgCount = cnt;
      s_chatLastRxMs = lastRx;
      s_chatGpsState = gpsState;
      return changed;
    }
    case Screen::DEVICES: {
      uint32_t h = 0;
      size_t n = meshDeviceCount();
      for (size_t i = 0; i < n; i++) {
        const DeviceEntry& e = meshDevices()[i];
        h = h * 31 + e.lastSeenMs;
        h = h * 31 + (uint32_t)(e.lastRssi * 10.0f);
        h = h * 31 + (e.online ? 1u : 0u) + (e.sosSeen ? 2u : 0u);
      }
      bool changed = (h != s_devHash) || (n != s_devCount);
      s_devHash = h;
      s_devCount = n;
      return changed;
    }
    case Screen::STATUS: {
      const GnssData& g = gnssData();
      uint32_t h = (uint32_t)(g.lat * 10.0f) * 7 + (uint32_t)(g.lon * 10.0f) * 13 +
                   g.sats + (g.valid ? 0x1000u : 0u) +
                   (storeAvailable() ? 0x2000u : 0u) + (uint32_t)meshDeviceCount() +
                   (uint32_t)meshGetPower() + (uint32_t)meshGetSf();
      bool changed = (h != s_statusHash);
      s_statusHash = h;
      return changed;
    }
    case Screen::WAYPOINTS: {
      size_t n = storeWaypointCount();
      bool changed = (n != s_waypointCount);
      s_waypointCount = n;
      return changed;
    }
    default:
      return false;
  }
}

// ---------------- 渲染入口 ----------------
void uiRender() {
  uint32_t now = millis();

  bool need = s_dirty || uiDataChanged();
  if (!need) return;
  s_dirty = false;

  if (s_sosOvVisible) {
    renderSosOverlay();
  } else {
    switch (s_screen) {
      case Screen::CHAT:     renderChat(); break;
      case Screen::DEVICES:  renderDevices(); break;
      case Screen::STATUS:   renderStatus(); break;
      case Screen::WAYPOINTS: renderWaypoints(); break;
      case Screen::SETTINGS: renderSettings(); break;
    }

    if (s_toast[0] != '\0' && now < s_toastUntil) {
      D().fillRect(0, SCREEN_H - 40, SCREEN_W, 40, C_PANEL2);
      D().drawRect(0, SCREEN_H - 40, SCREEN_W, 40, C_LINE);
      D().setFont(F_MINI);
      D().setTextDatum(top_left);
      D().setTextColor(C_YELLOW, C_PANEL2);
      D().drawString(s_toast, 4, SCREEN_H - 36);
    }
  }

  // 一次性把整帧推送到屏幕（双缓冲，无闪烁）
  if (s_canvas) {
    s_canvas->pushSprite(0, 0);
  }
}

Screen uiScreen() { return s_screen; }

void uiGoto(Screen s) {
  s_screen = s;
  s_sel = 0;
  s_scrollPx = 0;
  s_atBottom = true;
  s_editing = false;
  uiInputClear();
  s_dirty = true;
}

void uiInputAppend(char c) {
  if (c == '\0') return;
  if (s_inputLen >= MSG_MAX_INPUT) return;
  s_input[s_inputLen++] = c;
  s_input[s_inputLen] = '\0';
  s_dirty = true;
}

void uiInputBackspace() {
  if (s_inputLen == 0) return;
  // 回退一个 UTF-8 字符
  int i = (int)s_inputLen - 1;
  while (i > 0 && ((uint8_t)s_input[i] & 0xC0) == 0x80) i--;
  s_input[i] = '\0';
  s_inputLen = (size_t)i;
  s_dirty = true;
}

void uiInputClear() {
  s_input[0] = '\0';
  s_inputLen = 0;
  s_dirty = true;
}

const char* uiInputText() { return s_input; }

void uiChatScrollBy(int delta) {
  s_scrollPx += delta;
  s_atBottom = false;
  s_dirty = true;
}

void uiChatScrollToBottom() {
  s_scrollPx = 0x7FFFFFFF;
  s_atBottom = true;
  s_dirty = true;
}

void uiSelMove(int delta) {
  s_sel += delta;
  if (s_sel < 0) s_sel = 0;
  s_dirty = true;
}

int uiSel() { return s_sel; }

bool uiSettingEditing() { return s_editing; }
void uiSettingStartEdit() {
  s_editing = true;
  uiInputClear();
  s_dirty = true;
}

void uiSettingCancelEdit() {
  s_editing = false;
  uiInputClear();
  s_dirty = true;
}

void uiSettingCommitEdit() {
  const char* v = uiInputText();
  if (v[0] == '\0') { uiSettingCancelEdit(); return; }
  switch (s_sel) {
    case 0: meshSetName(v); break;
    case 1: meshSetAesKey(v); break;
    case 2: {
      float f = atof(v);
      if (f < LORA_FREQ_MIN) f = LORA_FREQ_MIN;
      if (f > LORA_FREQ_MAX) f = LORA_FREQ_MAX;
      meshSetFreq(f);
      break;
    }
    case 3: {
      int p = atoi(v);
      if (p < LORA_POWER_MIN) p = LORA_POWER_MIN;
      if (p > LORA_POWER_MAX) p = LORA_POWER_MAX;
      meshSetPower((int8_t)p);
      break;
    }
    case 4: {
      int sf = atoi(v);
      if (sf < LORA_SF_MIN) sf = LORA_SF_MIN;
      if (sf > LORA_SF_MAX) sf = LORA_SF_MAX;
      meshSetSf((uint8_t)sf);
      break;
    }
    case 5: {
      int sw = (int)strtol(v, nullptr, 0);  // 支持 0x 十六进制
      if (sw < 0) sw = 0;
      if (sw > 255) sw = 255;
      meshSetSyncWord((uint8_t)sw);
      break;
    }
    default: break;
  }
  s_editing = false;
  uiInputClear();
  s_dirty = true;
}

void uiSosOverlayShow(uint32_t source, bool local) {
  s_sosOvVisible = true;
  s_sosOvSource = source;
  s_sosOvLocal = local;
  s_dirty = true;
}

void uiSosOverlayHide() {
  s_sosOvVisible = false;
  s_dirty = true;
}

bool uiSosOverlayVisible() { return s_sosOvVisible; }

void uiToast(const char* text) {
  strncpy(s_toast, text, sizeof(s_toast) - 1);
  s_toast[sizeof(s_toast) - 1] = '\0';
  s_toastUntil = millis() + 1500;
  s_dirty = true;
}
