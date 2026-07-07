#include "HomeScreen.h"
#include "TimeManager.h"
#include "Display.h"
#include "Config.h"

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

extern Adafruit_ST7789 tft;

int lastSecond = -1;
int lastDay = -1;

String twoDigits(int value) {
  if (value < 10) {
    return "0" + String(value);
  }
  return String(value);
}

void drawHomeScreen() {
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_CYAN);
  tft.setTextSize(2);
  tft.setCursor(20, 15);
  tft.print("Habit Tracker");

  tft.drawLine(0, 45, 320, 45, ST77XX_WHITE);

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(70, 170);
  tft.print("Press ACTION");

  lastSecond = -1;
  lastDay = -1;
  updateHomeScreen();
}

void updateHomeScreen() {
  updateTime();

  int currentSecond = getSecond();

  if (currentSecond != lastSecond) {
    lastSecond = currentSecond;

    String timeText =
      twoDigits(getHour()) + ":" +
      twoDigits(getMinute()) + ":" +
      twoDigits(getSecond());

    // Clear only the tight box the time text occupies, not the whole row —
    // a full-row clear every second is what caused the visible flicker.
    tft.fillRect(55, 70, 200, 34, ST77XX_BLACK);

    tft.setTextColor(ST77XX_GREEN);
    tft.setTextSize(4);
    tft.setCursor(55, 70);
    tft.print(timeText);
  }

  int currentDay = getDay();

  if (currentDay != lastDay) {
    lastDay = currentDay;

    String dateText =
      getWeekday() + ", " +
      String(getDay()) + " " +
      getMonthName() + " " +
      String(getYear());

    // Only redraw the date when the day actually changes, instead of every second.
    tft.fillRect(0, 125, 320, 20, ST77XX_BLACK);

    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.setCursor(25, 125);
    tft.print(dateText);
  }
}