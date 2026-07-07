#include "Config.h"
#include "Input.h"
#include "Display.h"
#include "HabitTrackerScreen.h"
#include "StatsScreen.h"
#include "MonthSelectScreen.h"
#include "PomodoroScreen.h"
#include "TimeManager.h"
#include "AudioManager.h"
#include "SettingsScreen.h"
#include <Arduino.h>
#include "HomeScreen.h"

enum Screen {
  HOME,
  MAIN_MENU,
  HABIT_TRACKER,
  MONTH_SELECT,
  STATS,
  DAY_DETAIL,
  POMODORO_SETUP,
  POMODORO_TIMER,
  SETTINGS
};

Screen currentScreen = HOME;

const char* menuItems[] = {
  "Habit Tracker",
  "Pomodoro",
  "Stats",
  "Settings"
};

const int menuCount = 4;

int selectedItem = 0;
bool inSubScreen = false;

void setup() {
  Serial.begin(115200);
  setupInput();
  setupDisplay();
  setupAudio();
  setupSettingsStorage();

  drawHomeScreen();

  setupTime();
  setupHabitStorage();

  drawHomeScreen();
}

void loop() {

  int move = getEncoderMove();

  switch(currentScreen) {
  case HOME:
    updateHomeScreen();

    if (isActionPressed()) {
      currentScreen = MAIN_MENU;
      drawMainMenu(selectedItem, menuItems, menuCount);
    }

    break;

    case MAIN_MENU:

      if (move != 0) {
    int oldItem = selectedItem;

    selectedItem += move;

    if (selectedItem < 0)
        selectedItem = menuCount - 1;

    if (selectedItem >= menuCount)
        selectedItem = 0;

    drawMenuItem(oldItem, false, menuItems);
    drawMenuItem(selectedItem, true, menuItems);
      }

      if (isActionPressed()) {

        switch(selectedItem) {

          case 0: // Habit Tracker
            currentScreen = HABIT_TRACKER;
            drawHabitTracker();
            break;

          case 1:
            currentScreen = POMODORO_SETUP;
            drawPomodoroSetupScreen();
            break;

          case 2:
            currentScreen = MONTH_SELECT;
            drawMonthSelectScreen();
            break;

          case 3:
            currentScreen = SETTINGS;
            drawSettingsScreen();
            break;
        }
      }
      if (isBackPressed()) {
        currentScreen = HOME;
        drawHomeScreen();
      }

      break;

    case HABIT_TRACKER:
    if (isBackPressed()) {

    currentScreen = MAIN_MENU;

    drawMainMenu(
      selectedItem,
      menuItems,
      menuCount
    );
  }

      if (move != 0) {
        habitTrackerMove(move);
      }

      if (isActionPressed()) {
        habitTrackerToggle();
      }

      break;

    case MONTH_SELECT:
      if (isBackPressed()) {
        currentScreen = MAIN_MENU;
        drawMainMenu(selectedItem, menuItems, menuCount);
      }

      if (move != 0) {
        monthSelectMove(move);
      }

      if (isButtonPressed()) {
        monthSelectToggleFocus();
      }

      if (isActionPressed()) {
        currentScreen = STATS;
        drawStatsScreenForMonth(getMonthSelectYear(), getMonthSelectMonth());
      }

      break;

      case STATS:
    if (isBackPressed()) {
      currentScreen = MONTH_SELECT;
      redrawMonthSelectScreen();
    }
    if (move != 0) {
        statsMoveDay(move);
      }

    if (isActionPressed()) {
      currentScreen = DAY_DETAIL;
      drawDayDetailScreen(getSelectedDay());
    }

    break;

    case DAY_DETAIL:
      if (isBackPressed()) {
        currentScreen = STATS;
        redrawStatsScreen();
  }
  break;

    case POMODORO_SETUP:
      if (isBackPressed()) {
        currentScreen = MAIN_MENU;
        drawMainMenu(selectedItem, menuItems, menuCount);
      }

      if (move != 0) {
        pomodoroSetupMove(move);
      }

      if (isButtonPressed()) {
        pomodoroSetupToggleFocus();
      }

      if (isActionPressed()) {
        currentScreen = POMODORO_TIMER;
        startPomodoroTimer();
      }

      break;

    case POMODORO_TIMER:
      updatePomodoroTimer();

      if (isActionPressed()) {
        togglePomodoroPause();
      }

      if (isBackPressed()) {
        currentScreen = POMODORO_SETUP;
        drawPomodoroSetupScreen();
      }

      break;

    case SETTINGS:
      if (move != 0) {
        settingsMove(move);
      }

      if (isActionPressed()) {
        settingsToggleFocus();
      }

      if (isBackPressed()) {
        saveSettings();
        currentScreen = MAIN_MENU;
        drawMainMenu(selectedItem, menuItems, menuCount);
      }

      break;
  }
}