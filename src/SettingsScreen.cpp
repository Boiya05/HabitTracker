#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Preferences.h>

#include "SettingsScreen.h"
#include "StatsScreen.h" // for drawThickRect
#include "Display.h"
#include "AudioManager.h"
#include "TimeManager.h"

extern Adafruit_ST7789 tft;

static Preferences settingsPrefs;

enum SettingsField { FIELD_BRIGHTNESS = 0, FIELD_VOLUME = 1, FIELD_UTC_OFFSET = 2 };

static const int BRIGHTNESS_STEP = 10;
static const int VOLUME_STEP = 10;
static const int UTC_OFFSET_MIN = -12;
static const int UTC_OFFSET_MAX = 14;

static SettingsField focusedField = FIELD_BRIGHTNESS;

static const int fieldBoxX = 40;
static const int fieldBoxW = 240;
static const int fieldBoxH = 40;
static const int fieldSpacing = 55;
static const int fieldStartY = 60;

static int fieldBoxY(int index) {
  return fieldStartY + index * fieldSpacing;
}

static void drawFieldBorder(int index) {
  int y = fieldBoxY(index);
  uint16_t color = ((int)focusedField == index) ? ST77XX_CYAN : ST77XX_WHITE;
  drawThickRect(fieldBoxX, y, fieldBoxW, fieldBoxH, color, 2);
}

static void drawFieldLabel(int index, const char* label) {
  int y = fieldBoxY(index);
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(fieldBoxX + 6, y - 12);
  tft.print(label);
}

static void drawFieldValue(int index) {
  int y = fieldBoxY(index);

  tft.fillRect(fieldBoxX + 6, y + 8, fieldBoxW - 12, 24, ST77XX_BLACK);

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(fieldBoxX + 10, y + 12);

  switch (index) {
    case FIELD_BRIGHTNESS:
      tft.print(getBacklightPercent());
      tft.print("%");
      break;

    case FIELD_VOLUME:
      tft.print(getVolumePercent());
      tft.print("%");
      break;

    case FIELD_UTC_OFFSET: {
      int offset = getUtcOffsetHours();
      tft.print("UTC");
      tft.print(offset >= 0 ? "+" : "");
      tft.print(offset);
      break;
    }
  }
}

void setupSettingsStorage() {
  settingsPrefs.begin("settings", false);

  setBacklightPercent(settingsPrefs.getInt("brightness", 100));
  setVolumePercent(settingsPrefs.getInt("volume", 70));
  setUtcOffsetHours(settingsPrefs.getInt("utcOffset", 8));
}

void saveSettings() {
  settingsPrefs.putInt("brightness", getBacklightPercent());
  settingsPrefs.putInt("volume", getVolumePercent());
  settingsPrefs.putInt("utcOffset", getUtcOffsetHours());

  applyUtcOffset(); // reflect a time zone change immediately, no reboot needed
}

void drawSettingsScreen() {
  focusedField = FIELD_BRIGHTNESS;

  tft.fillScreen(ST77XX_BLACK);

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(15, 12);
  tft.print("Settings");

  tft.drawLine(0, 40, 320, 40, ST77XX_WHITE);

  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(15, 220);
  tft.print("Turn: adjust   Action: next field   Back: save");

  drawFieldLabel(FIELD_BRIGHTNESS, "Brightness");
  drawFieldLabel(FIELD_VOLUME, "Volume");
  drawFieldLabel(FIELD_UTC_OFFSET, "Time Zone");

  for (int i = 0; i < 3; i++) {
    drawFieldBorder(i);
    drawFieldValue(i);
  }
}

void settingsMove(int direction) {
  switch (focusedField) {
    case FIELD_BRIGHTNESS:
      setBacklightPercent(getBacklightPercent() + direction * BRIGHTNESS_STEP);
      drawFieldValue(FIELD_BRIGHTNESS);
      break;

    case FIELD_VOLUME:
      setVolumePercent(getVolumePercent() + direction * VOLUME_STEP);
      drawFieldValue(FIELD_VOLUME);
      playTone(1000, 80); // preview the new volume immediately
      break;

    case FIELD_UTC_OFFSET: {
      int offset = getUtcOffsetHours() + direction;
      if (offset < UTC_OFFSET_MIN) offset = UTC_OFFSET_MIN;
      if (offset > UTC_OFFSET_MAX) offset = UTC_OFFSET_MAX;
      setUtcOffsetHours(offset);
      drawFieldValue(FIELD_UTC_OFFSET);
      break;
    }
  }
}

void settingsToggleFocus() {
  int oldField = focusedField;
  focusedField = (SettingsField)((focusedField + 1) % 3);

  drawFieldBorder(oldField);
  drawFieldBorder(focusedField);
}
