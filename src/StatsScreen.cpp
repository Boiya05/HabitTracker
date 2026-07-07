#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Preferences.h>

#include "StatsScreen.h"
#include "Config.h"
#include "TimeManager.h"

extern Preferences prefs;
extern Adafruit_ST7789 tft;

int selectedDay = 1;

// Month being viewed in stats
int viewedYear;
int viewedMonth;
String viewedMonthName;

// Calendar UI layout
int startX = 25;
int startY = 75;
int squareSize = 18;
int xspacing = 35;
int yspacing = 30;

const char* statWeekdays[] = {
  "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};

String makeHabitKeyForDate(int year, int month, int day, int habitIndex) {
  return String(year) + "-" +
         String(month) + "-" +
         String(day) + "-h" +
         String(habitIndex);
}

int getSelectedDay() {
  return selectedDay;
}

void drawStatsScreen() {
  drawStatsScreenForMonth(getYear(), getMonth());
}

void drawStatsScreenForMonth(int year, int month) {
  viewedYear = year;
  viewedMonth = month;
  viewedMonthName = getMonthNameFor(month);

  // Default to today's day if we're viewing the current month, otherwise start at day 1.
  if (year == getYear() && month == getMonth()) {
    selectedDay = getDay();
  } else {
    selectedDay = 1;
  }

  int daysInMonth = getDaysInMonth(viewedYear, viewedMonth);
  if (selectedDay < 1) selectedDay = 1;
  if (selectedDay > daysInMonth) selectedDay = daysInMonth;

  redrawStatsScreen();
}

// Re-entry (e.g. backing out of the day-detail screen): keep the current viewed
// month/year and selected day instead of resetting to today.
void redrawStatsScreen() {
  tft.fillScreen(ST77XX_BLACK);

  drawDate();
  drawDay();
  drawCalendarUI();
  drawSelectedDayInfo();
}

void statsMoveDay(int direction) {
  int oldDay = selectedDay;

  selectedDay += direction;

  int daysInMonth = getDaysInMonth(viewedYear, viewedMonth);
  if (selectedDay < 1)              selectedDay = daysInMonth;
  if (selectedDay > daysInMonth)    selectedDay = 1;

  drawDAYselectBOX(oldDay, false);
  drawDAYselectBOX(selectedDay, true);
}

void drawDayDetailScreen(int day) {
  int firstWeekday = getFirstWeekdayOfMonth(viewedYear, viewedMonth);
  int weekdayIndex = (firstWeekday + day - 1) % 7;

  tft.fillScreen(ST77XX_BLACK);

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(15, 15);
  tft.println("Day Details");

  tft.setCursor(15, 40);
  tft.print(statWeekdays[weekdayIndex]);
  tft.print(" ");
  tft.print(day);
  tft.print("-");
  tft.print(viewedMonth);
  tft.print("-");
  tft.print(viewedYear);

  int txtlnspacing = 20;

  for (int nohabit = 0; nohabit < HABIT_COUNT; nohabit++) {
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(15, 80 + nohabit * txtlnspacing);

    tft.print(habitNames[nohabit]);
    tft.print(" : ");

    String key = makeHabitKeyForDate(viewedYear, viewedMonth, day, nohabit);
    bool isDone = prefs.getBool(key.c_str(), false);

    if (isDone) {
      tft.setTextColor(ST77XX_GREEN);
      tft.print("Done");
    } else {
      tft.setTextColor(ST77XX_RED);
      tft.print("Failed");
    }
  }
}

void drawDAYselectBOX(int day, bool selected) {
  int firstWeekday = getFirstWeekdayOfMonth(viewedYear, viewedMonth);
  int col = (firstWeekday + (day - 1)) % 7;
  int row = (firstWeekday + (day - 1)) / 7;

  int x = startX + col * xspacing;
  int y = startY + row * yspacing;

  if (!selected) {
    drawThickRect(x - 2, y - 2, squareSize + 4, squareSize + 4, ST77XX_BLACK, 3);
  } else {
    drawThickRect(x - 2, y - 2, squareSize + 4, squareSize + 4, ST77XX_CYAN, 3);
    drawSelectedDayInfo();
  }
}

void drawSelectedDayInfo() {
  int firstWeekday = getFirstWeekdayOfMonth(viewedYear, viewedMonth);
  int weekdayIndex = (firstWeekday + selectedDay - 1) % 7;

  tft.fillRect(220, 10, 100, 45, ST77XX_BLACK);

  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE);

  tft.setCursor(230, 15);
  tft.println(statWeekdays[weekdayIndex]);

  tft.setCursor(230, 30);
  tft.print(selectedDay);
  tft.print("-");
  tft.print(viewedMonth);
}

void drawDate() {
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(15, 15);
  tft.print(viewedMonthName);
  tft.print(" ");
  tft.println(viewedYear);
}

void drawDay() {
  const char* days[] = {"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_CYAN);

  for (int i = 0; i < 7; i++) {
    // at textSize=1, each char is 6px — "Mon" = 18px wide = same as squareSize
    // so startX + i*xspacing left-aligns the label perfectly with the square
    tft.setCursor(startX + i * xspacing, 55);
    tft.print(days[i]);
  }
}
void drawCalendarUI() {
  int daysInMonth  = getDaysInMonth(viewedYear, viewedMonth);
  int firstWeekday = getFirstWeekdayOfMonth(viewedYear, viewedMonth); // Mon=0

  for (int day = 1; day <= daysInMonth; day++) {  // was <= 30
    int completedHabits = 0;

    // ---- FIX: offset by where the 1st actually lands ----
    int col = (firstWeekday + (day - 1)) % 7;
    int row = (firstWeekday + (day - 1)) / 7;
    // -----------------------------------------------------

    int x = startX + col * xspacing;
    int y = startY + row * yspacing;

    for (int habit = 0; habit < HABIT_COUNT; habit++) {
      String key = makeHabitKeyForDate(viewedYear, viewedMonth, day, habit);
      if (prefs.getBool(key.c_str(), false)) completedHabits++;
    }

    if      (completedHabits == 0)          tft.fillRect(x, y, squareSize, squareSize, ST77XX_RED);
    else if (completedHabits < HABIT_COUNT) tft.fillRect(x, y, squareSize, squareSize, ST77XX_YELLOW);
    else                                    tft.fillRect(x, y, squareSize, squareSize, ST77XX_GREEN);

    if (day == selectedDay) {
      drawThickRect(x - 2, y - 2, squareSize + 4, squareSize + 4, ST77XX_CYAN, 3);
    }
  }
}
void drawThickRect(int x, int y, int w, int h, uint16_t color, int thickness) {
  for (int i = 0; i < thickness; i++) {
    tft.drawRect(x + i, y + i, w - 2 * i, h - 2 * i, color);
  }
}