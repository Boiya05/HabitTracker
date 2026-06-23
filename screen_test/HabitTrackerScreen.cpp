#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "HabitTrackerScreen.h"
#include <Preferences.h>

Preferences prefs;

// This uses the TFT object created in Display.cpp
extern Adafruit_ST7789 tft;

const int HABIT_COUNT = 4;

const char* habits[HABIT_COUNT] = {
  "Gym",
  "Reading",
  "Piano",
  "Project"
};

bool habitDone[HABIT_COUNT] = {
  false,
  false,
  false,
  false
};

int selectedHabit = 0;

void drawHabitTracker() {
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(15, 15);
  tft.println("Sun 21 Jun 2026");

  tft.drawLine(0, 45, 320, 45, ST77XX_WHITE);

  tft.setTextSize(2);

  for (int i = 0; i < HABIT_COUNT; i++) {
    int y = 70 + i * 35;

    if (i == selectedHabit) {
      tft.fillRect(10, y - 6, 300, 28, ST77XX_BLUE);
    }

    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(25, y);

    if (i == selectedHabit) {
      tft.print("> ");
    } else {
      tft.print("  ");
    }

    if (habitDone[i]) {
      tft.print("[x] ");
    } else {
      tft.print("[ ] ");
    }

    tft.println(habits[i]);
  }
}

void habitTrackerToggle() {
  habitDone[selectedHabit] = !habitDone[selectedHabit];

  String key = "h" + String(selectedHabit);
  prefs.putBool(key.c_str(), habitDone[selectedHabit]);

  drawHabitTracker();
}

void setupHabitStorage() {
  prefs.begin("habits", false);

  for (int i = 0; i < HABIT_COUNT; i++) {
    String key = "h" + String(i);
    habitDone[i] = prefs.getBool(key.c_str(), false);
  }
}

void habitTrackerMove(int direction) {
  selectedHabit += direction;

  if (selectedHabit < 0) {
    selectedHabit = HABIT_COUNT - 1;
  }

  if (selectedHabit >= HABIT_COUNT) {
    selectedHabit = 0;
  }

  drawHabitTracker();
}