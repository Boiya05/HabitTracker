#include "Config.h"
#include "Input.h"
#include "Display.h"
#include "HabitTrackerScreen.h"
#include "StatsScreen.h"
#include "TimeManager.h"
#include "HomeScreen.h"

enum Screen {
  HOME,
  MAIN_MENU,
  HABIT_TRACKER,
  STATS,
  DAY_DETAIL
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
            Serial.println("Pomodoro");
            break;

          case 2:
            currentScreen = STATS;
            drawStatsScreen();
            break;

          case 3:
            Serial.println("Settings");
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

      case STATS:
    if (isBackPressed()) {
      currentScreen = MAIN_MENU;
      drawMainMenu(selectedItem, menuItems, menuCount);
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
        drawStatsScreen();
  }
  break;
  }
}