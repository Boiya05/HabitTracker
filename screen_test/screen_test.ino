#include "Config.h"
#include "Input.h"
#include "Display.h"
#include "Config.h"
#include "Input.h"
#include "Display.h"
#include "HabitTrackerScreen.h"

enum Screen {
  MAIN_MENU,
  HABIT_TRACKER
  STATS
};

Screen currentScreen = MAIN_MENU;

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
  setupHabitStorage();

  drawMainMenu(selectedItem, menuItems, menuCount);}

void loop() {

  int move = getEncoderMove();

  switch(currentScreen) {

    case MAIN_MENU:

      if (move != 0) {

        selectedItem += move;

        if (selectedItem < 0)
          selectedItem = menuCount - 1;

        if (selectedItem >= menuCount)
          selectedItem = 0;

        drawMainMenu(selectedItem, menuItems, menuCount);
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
            break;

          case 3:
            Serial.println("Settings");
            break;
        }
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
  }
}