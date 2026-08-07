// ============================================================
// gnss.cpp - ATGM336H GNSS 定位（NMEA 解析）
// ============================================================

#include "gnss.h"

#include <Arduino.h>
#include <cstring>

#include "config.h"

static GnssData s_gnss;

// 将 NMEA 坐标 ddmm.mmmmm（含半球）转为十进制度
static bool parseNmeaCoord(const char* field, char hemi, float* out) {
  if (field == nullptr || field[0] == '\0') return false;
  double ddmm = atof(field);
  int32_t deg = (int32_t)(ddmm / 100.0);
  double min  = ddmm - deg * 100.0;
  double val  = deg + min / 60.0;
  if (hemi == 'S' || hemi == 'W') val = -val;
  *out = (float)val;
  return true;
}

static uint8_t countFields(const char* line, char sep) {
  uint8_t n = 0;
  for (const char* p = line; *p; p++) {
    if (*p == sep) n++;
  }
  return n + 1;
}

// 从逗号分隔的句子中取第 idx 个字段（0 起）
static const char* fieldAt(const char* line, uint8_t idx) {
  const char* p = line;
  while (idx-- > 0) {
    p = strchr(p, ',');
    if (!p) return nullptr;
    p++;
  }
  return p;
}

static uint8_t fieldLen(const char* field) {
  const char* end = strchr(field, ',');
  if (!end) end = field + strlen(field);
  return (uint8_t)(end - field);
}

static void parseRmc(const char* line) {
  // $xxRMC,time,status,lat,N,lon,E,speed,course,date,magvar,magdir,mode
  const char* status = fieldAt(line, 2);
  const char* lat    = fieldAt(line, 3);
  const char* latH   = fieldAt(line, 4);
  const char* lon    = fieldAt(line, 5);
  const char* lonH   = fieldAt(line, 6);

  if (!status || fieldLen(status) != 1 || status[0] != 'A') {
    s_gnss.valid = false;
    return;
  }
  if (!lat || !latH || !lon || !lonH) return;

  float la, lo;
  if (!parseNmeaCoord(lat, latH[0], &la)) return;
  if (!parseNmeaCoord(lon, lonH[0], &lo)) return;

  s_gnss.lat      = la;
  s_gnss.lon      = lo;
  s_gnss.valid    = true;
  s_gnss.updated  = true;
  s_gnss.lastFixMs = millis();
}

static void parseGga(const char* line) {
  // $xxGGA,time,lat,N,lon,E,fix,sats,hdop,...
  const char* lat    = fieldAt(line, 2);
  const char* latH   = fieldAt(line, 3);
  const char* lon    = fieldAt(line, 4);
  const char* lonH   = fieldAt(line, 5);
  const char* fix    = fieldAt(line, 6);
  const char* sats   = fieldAt(line, 7);

  if (!lat || !latH || !lon || !lonH || !fix) return;

  uint8_t fixQ = (uint8_t)atoi(fix);
  if (fixQ < 1) return;  // 无定位

  float la, lo;
  if (!parseNmeaCoord(lat, latH[0], &la)) return;
  if (!parseNmeaCoord(lon, lonH[0], &lo)) return;

  s_gnss.lat      = la;
  s_gnss.lon      = lo;
  s_gnss.valid    = true;
  s_gnss.updated  = true;
  s_gnss.lastFixMs = millis();
  if (sats) s_gnss.sats = (uint8_t)atoi(sats);
}

static void processLine(const char* line, size_t len) {
  // 取句类型（$后两字节）
  if (len < 6 || line[0] != '$') return;
  char talker = line[1];
  char type   = line[2];
  if (talker != 'G' && talker != 'P') return;

  switch (type) {
    case 'R':
      parseRmc(line);       // xxRMC
      break;
    case 'G':
      if (line[3] == 'G' && line[4] == 'A') {
        parseGga(line);     // xxGGA
      }
      break;
    default:
      break;
  }
}

bool gnssInit() {
  Serial1.begin(GPS_BAUD, SERIAL_8N1, PIN_GPS_RX, PIN_GPS_TX);
  return true;
}

void gnssUpdate() {
  static char   line[160];
  static size_t pos = 0;

  while (Serial1.available()) {
    char c = (char)Serial1.read();
    if (c == '\n') {
      line[pos] = '\0';
      if (pos > 0 && line[pos - 1] == '\r') line[pos - 1] = '\0';
      processLine(line, pos);
      pos = 0;
    } else if (pos < sizeof(line) - 1) {
      line[pos++] = c;
    }
  }
}

const GnssData& gnssData() {
  return s_gnss;
}
