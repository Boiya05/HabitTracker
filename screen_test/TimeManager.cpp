#include "TimeManager.h"
#include <WiFi.h>

// ===== WiFi =====
const char* ssid = "bobbi34@unifi";
const char* password = "0162735254";

// ===== NTP =====
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 8 * 3600;   // Malaysia
const int daylightOffset_sec = 0;

// Stores current time
struct tm timeInfo;
String getDateKey() {
  return String(getYear()) + "-" + String(getMonth()) + "-" + String(getDay());
}

void setupTime() {
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  unsigned long wifiStart = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < 15000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWiFi failed");
    return;
  }

  Serial.println("\nWiFi connected");

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  Serial.print("Syncing NTP");

  unsigned long ntpStart = millis();

  while (!getLocalTime(&timeInfo) && millis() - ntpStart < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (!getLocalTime(&timeInfo)) {
    Serial.println("\nNTP failed");
    return;
  }

  Serial.println("\nTime synced!");
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

    return months[timeInfo.tm_mon];
}