// ============================================================
// store.cpp - 持久化（聊天日志 -> SD 卡；航点 -> NVS）
// ============================================================

#include "store.h"

#include <Arduino.h>
#include <Preferences.h>
#include <SD.h>
#include <cstring>

#include "config.h"
#include "radio.h"

static bool s_sdOk = false;

static const char* LOG_PATH = "/mesh/log.txt";

bool storeBegin() {
  if (!SD.begin(PIN_SD_CS, radioSPI(), 25000000)) {
    s_sdOk = false;
    return false;
  }
  if (SD.cardType() == CARD_NONE) {
    s_sdOk = false;
    return false;
  }
  SD.mkdir("/mesh");
  s_sdOk = true;
  return true;
}

bool storeAvailable() { return s_sdOk; }

void storeLogMessage(const ChatMsg& m) {
  if (!s_sdOk) return;
  File f = SD.open(LOG_PATH, FILE_APPEND);
  if (!f) return;

  // I|O,type,srcid,rssi,lat,lon,time,text
  f.printf("%s,%u,%u,%d,%.6f,%.6f,%lu,",
           m.incoming ? "I" : "O",
           (unsigned)m.type,
           (unsigned)m.srcId,
           (int)m.rssi,
           (double)m.lat,
           (double)m.lon,
           (unsigned long)(m.rxMs / 1000));
  f.print(m.text);
  f.print("\n");
  f.close();
}

void storeLoadHistory(size_t maxCount) {
  if (!s_sdOk) return;
  File f = SD.open(LOG_PATH, FILE_READ);
  if (!f) return;

  // 逐行读取，只保留末尾 maxCount 条
  ChatMsg lines[MAX_MESSAGES];
  size_t  n = 0;
  char    line[320];

  while (f.available() && n < MAX_MESSAGES) {
    size_t i = 0;
    while (f.available() && i < sizeof(line) - 1) {
      char c = (char)f.read();
      if (c == '\n') break;
      line[i++] = c;
    }
    line[i] = '\0';
    if (i == 0) continue;

    ChatMsg m;
    char*   save = nullptr;
    char*   tok  = strtok_r(line, ",", &save);
    int     col  = 0;
    while (tok) {
      switch (col) {
        case 0: m.incoming = (tok[0] == 'I'); break;
        case 1: m.type     = (uint8_t)atoi(tok); break;
        case 2: m.srcId    = (uint32_t)strtoul(tok, nullptr, 10); break;
        case 3: m.rssi     = (float)atoi(tok); break;
        case 4: m.lat      = (float)atof(tok); m.hasLocation = true; break;
        case 5: m.lon      = (float)atof(tok); break;
        case 6: m.rxMs     = (uint32_t)strtoul(tok, nullptr, 10) * 1000UL; break;
        case 7: {
          // 剩余全部为正文（可能含逗号）
          size_t tl = strlen(tok);
          if (tl > MSG_MAX_LEN) tl = MSG_MAX_LEN;
          memcpy(m.text, tok, tl);
          m.text[tl] = '\0';
          break;
        }
        default: break;
      }
      if (col == 7) break;
      col++;
      tok = strtok_r(nullptr, ",", &save);
    }
    if (m.type != MSG_BEACON) {
      if (n < MAX_MESSAGES) lines[n++] = m;
    }
  }
  f.close();

  // 保留末尾 maxCount 条
  size_t start = (n > maxCount) ? (n - maxCount) : 0;
  for (size_t k = start; k < n; k++) {
    meshImportMessage(lines[k]);
  }
}

// ---------------- 航点（坐标点记录，存 NVS，不依赖 SD 卡） ----------------
static Preferences s_wp;

bool storeAddWaypoint(const char* name, float lat, float lon) {
  s_wp.begin("wp", false);
  uint8_t cnt = s_wp.getUChar("cnt", 0);
  if (cnt >= WAYPOINT_MAX) {
    s_wp.end();
    return false;
  }
  char key[8];
  char val[64];
  snprintf(key, sizeof(key), "w%u", (unsigned)cnt);
  snprintf(val, sizeof(val), "%s,%.6f,%.6f", name, (double)lat, (double)lon);
  s_wp.putString(key, val);
  s_wp.putUChar("cnt", cnt + 1);
  s_wp.end();
  return true;
}

size_t storeWaypointCount() {
  s_wp.begin("wp", true);
  uint8_t cnt = s_wp.getUChar("cnt", 0);
  s_wp.end();
  return cnt;
}

bool storeWaypointAt(size_t idx, char* name, size_t nameCap, float* lat, float* lon) {
  s_wp.begin("wp", true);
  char key[8];
  snprintf(key, sizeof(key), "w%u", (unsigned)idx);
  String val = s_wp.getString(key, "");
  s_wp.end();
  if (val.length() == 0) return false;
  int c1 = val.indexOf(',');
  int c2 = val.indexOf(',', c1 + 1);
  if (c1 <= 0 || c2 <= c1) return false;
  String nm = val.substring(0, c1);
  String la = val.substring(c1 + 1, c2);
  String lo = val.substring(c2 + 1);
  strncpy(name, nm.c_str(), nameCap - 1);
  name[nameCap - 1] = '\0';
  *lat = la.toFloat();
  *lon = lo.toFloat();
  return true;
}
