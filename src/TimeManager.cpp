#include "TimeManager.h"
#include <WiFi.h>

// ===== WiFi =====
const char* ssid = "bobbi34@unifi";
const char* password = "0162735254";

// ===== NTP =====
const char* ntpServer = "pool.ntp.org";
static long gmtOffsetSec = 8 * 3600; // default: Malaysia (UTC+8), overridden by Settings
const int daylightOffset_sec = 0;

// Stores current time
struct tm timeInfo;
String getDateKey() {
  return String(getYear()) + "-" + String(getMonth()) + "-" + String(getDay());
}

void setUtcOffsetHours(int hours) {
  if (hours < -12) hours = -12;
  if (hours > 14) hours = 14;
  gmtOffsetSec = (long)hours * 3600;
}

int getUtcOffsetHours() {
  return (int)(gmtOffsetSec / 3600);
}

// Re-applies the current offset to the already-synced clock, without waiting
// on a fresh WiFi/NTP round trip — used when leaving Settings so a time zone
// change is reflected immediately instead of needing a reboot.
void applyUtcOffset() {
  configTime(gmtOffsetSec, daylightOffset_sec, "my.pool.ntp.org", "asia.pool.ntp.org", ntpServer);
}

void setupTime() {
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  unsigned long wifiStart = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < 15000) {
    delay(200);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWiFi failed");
    return;
  }

  Serial.println("\nWiFi connected");

  // Country-level pool first (closest/fastest servers), falling back to
  // continent- and global-level pools if it doesn't respond.
  applyUtcOffset();

  Serial.println("Syncing NTP");

  // getLocalTime() already polls internally every 10ms up to its own timeout —
  // the old code wrapped it in an outer retry loop with delay(500), which on
  // every failed attempt paid that internal timeout AND the extra 500ms AND
  // started over, nearly doubling the worst-case wait. One call with the
  // full time budget does the same job without the redundant layer.
  if (!getLocalTime(&timeInfo, 10000)) {
    Serial.println("NTP failed");
    return;
  }

  Serial.println("Time synced!");
  Serial.println(&timeInfo, "%A, %B %d %Y %H:%M:%S");
}

void updateTime() {
    getLocalTime(&timeInfo);
}

int getHour() {
    return timeInfo.tm_hour;
}

int getMinute() {
    return timeInfo.tm_min;
}

int getSecond() {
    return timeInfo.tm_sec;
}

int getDay() {
    return timeInfo.tm_mday;
}

int getMonth() {
    return timeInfo.tm_mon + 1;
}

int getYear() {
    return timeInfo.tm_year + 1900;
}

String getWeekday() {

    const char* weekdays[] = {
        "Sunday",
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday",
        "Saturday"
    };

    return weekdays[timeInfo.tm_wday];
}

String getMonthName() {
    return getMonthNameFor(timeInfo.tm_mon + 1);
}

String getMonthNameFor(int month) {

    const char* months[] = {
        "January",
        "February",
        "March",
        "April",
        "May",
        "June",
        "July",
        "August",
        "September",
        "October",
        "November",
        "December"
    };

    return months[month - 1];
}

int getDaysInMonth(int year, int month) {
  if (month == 2) {
    bool leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    return leap ? 29 : 28;
  }

  if (month == 4 || month == 6 || month == 9 || month == 11) return 30;

  return 31;
}

// Zeller's congruence: weekday of the 1st of (year, month), returned Mon=0..Sun=6.
// Works for any year/month, unlike deriving it from the live clock struct.
int getFirstWeekdayOfMonth(int year, int month) {
  int m = month;
  int y = year;

  if (m < 3) {
    m += 12;
    y -= 1;
  }

  int K = y % 100;
  int J = y / 100;

  // h: 0=Saturday, 1=Sunday, 2=Monday, ... 6=Friday
  int h = (1 + (13 * (m + 1)) / 5 + K + K / 4 + J / 4 + 5 * J) % 7;

  return (h + 5) % 7; // convert to Mon=0..Sun=6
}