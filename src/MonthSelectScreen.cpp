#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

#include "MonthSelectScreen.h"
#include "StatsScreen.h"
#include "TimeManager.h"

extern Adafruit_ST7789 tft;

static const char* monthAbbrev[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static const int MIN_YEAR = 2000;
static const int MAX_YEAR = 2099;

static int selectedMonthIndex = 0; // 0-11 (Jan=0)
static int selectedYear = 2026;
static bool yearFocused = false;

// Month grid layout (4 cols x 3 rows)
static const int gridCols = 4;
static const int cellW = 55;
static const int cellH = 45;
static const int gridStartX = 15;
static const int gridStartY = 55;

// Year scroll box layout
static const int yearBoxX = 245;
static const int yearBoxY = 90;
static const int yearBoxW = 65;
static const int yearBoxH = 40;

static void monthCellOrigin(int index, int &x, int &y) {
  int col = index % gridCols;
  int row = index / gridCols;
  x = gridStartX + col * cellW;
  y = gridStartY + row * cellH;
}

// Clears the full area a cell can ever draw into, including the 1px-larger
// ring the highlight uses — the highlight and the plain border are NOT the
// same rect, so a redraw that only wipes the plain border's footprint leaves
// a ghost outline behind when the highlight moves off this cell.
static void drawMonthCellPlain(int index) {
  int x, y;
  monthCellOrigin(index, x, y);

  tft.fillRect(x - 1, y - 1, cellW - 2, cellH - 2, ST77XX_BLACK);
  tft.drawRect(x, y, cellW - 4, cellH - 4, ST77XX_WHITE);

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(x + 10, y + 13);
  tft.print(monthAbbrev[index]);
}

// Draws directly over the plain border (same rect, thicker) — safe to call
// without clearing first since it fully repaints every pixel of that ring.
static void drawMonthCellHighlight(int index) {
  int x, y;
  monthCellOrigin(index, x, y);

  uint16_t highlightColor = yearFocused ? ST77XX_BLUE : ST77XX_CYAN;
  drawThickRect(x - 1, y - 1, cellW - 2, cellH - 2, highlightColor, 2);
}

static void drawYearLabel() {
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(yearBoxX, yearBoxY - 14);
  tft.print("Year");
}

static void drawYearBorder() {
  uint16_t borderColor = yearFocused ? ST77XX_CYAN : ST77XX_WHITE;
  drawThickRect(yearBoxX, yearBoxY, yearBoxW, yearBoxH, borderColor, 2);
}

static void drawYearValue() {
  tft.fillRect(yearBoxX + 4, yearBoxY + 4, yearBoxW - 8, yearBoxH - 8, ST77XX_BLACK);

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(yearBoxX + 6, yearBoxY + 12);
  tft.print(selectedYear);
}

static void drawFullMonthGrid() {
  for (int i = 0; i < 12; i++) {
    drawMonthCellPlain(i);
  }
  drawMonthCellHighlight(selectedMonthIndex);
}

// Fresh entry (from the main menu): reset the selection to the current month/year.
void drawMonthSelectScreen() {
  selectedMonthIndex = getMonth() - 1;
  selectedYear = getYear();
  yearFocused = false;

  if (selectedMonthIndex < 0) selectedMonthIndex = 0;
  if (selectedMonthIndex > 11) selectedMonthIndex = 11;

  redrawMonthSelectScreen();
}

// Re-entry (e.g. backing out of the stats screen): keep whatever was last selected.
void redrawMonthSelectScreen() {
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(15, 12);
  tft.print("Select Month");

  tft.drawLine(0, 40, 320, 40, ST77XX_WHITE);

  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(15, 210);
  tft.print("Turn: move   Click: year   Press: view");

  drawFullMonthGrid();
  drawYearLabel();
  drawYearBorder();
  drawYearValue();
}

void monthSelectMove(int direction) {
  if (yearFocused) {
    int oldYear = selectedYear;

    selectedYear += direction;
    if (selectedYear < MIN_YEAR) selectedYear = MIN_YEAR;
    if (selectedYear > MAX_YEAR) selectedYear = MAX_YEAR;

    if (selectedYear != oldYear) {
      drawYearValue();
    }
  } else {
    int oldIndex = selectedMonthIndex;

    selectedMonthIndex += direction;
    if (selectedMonthIndex < 0)  selectedMonthIndex = 11;
    if (selectedMonthIndex > 11) selectedMonthIndex = 0;

    if (selectedMonthIndex != oldIndex) {
      drawMonthCellPlain(oldIndex);
      drawMonthCellHighlight(selectedMonthIndex);
    }
  }
}

void monthSelectToggleFocus() {
  yearFocused = !yearFocused;

  // Only the two focus rings change color — repaint just those, not the whole screen.
  drawMonthCellHighlight(selectedMonthIndex);
  drawYearBorder();
}

int getMonthSelectMonth() {
  return selectedMonthIndex + 1;
}

int getMonthSelectYear() {
  return selectedYear;
}
