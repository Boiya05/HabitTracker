#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "StatsScreen.h"

extern Adafruit_ST7789 tft;

String month = "Jun";
String year = "2026";//update later to real time

int selectedDay = 1;

void drawStatsScreen() {
  tft.fillScreen(ST77XX_BLACK);

  // Title/date
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(15, 15);
  tft.println(month + " " + year);
  
  // Day
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(20, 55);
  tft.println("Mon  Tue  Wed  Thu  Fri  Sat  Sun");


  // Calendar UI
  for(int day = 1; day<=30; day++){
    int startX = 25;
    int startY = 75;
    int squareSize = 18;
    int xspacing = 35;
    int yspacing = 30;
    int col = (day - 1) % 7;
    int row = (day - 1) / 7;
    int x = startX + col * xspacing;
    int y = startY + row * yspacing;
    tft.fillRect(x,y, squareSize, squareSize, ST77XX_WHITE);

    if (day ==  selectedDay){
      drawThickRect(x-2, y-2, squareSize+2, squareSize+2, ST77XX_GREEN,3);
    }
  }

}

void drawThickRect(int x, int y, int w, int h, uint16_t color, int thickness) {
  for (int i = 0; i < thickness; i++) {
    tft.drawRect(x + i, y + i, w - 2*i, h - 2*i, color);
  }
}

void statsMoveDay(int direction){
  selectedDay += direction;
  int numdays = 30; //update to real time in future

  if (selectedDay < 1) {
    selectedDay= numdays;
  }

  if (selectedDay > numdays) {
    selectedDay = 1;
  }

  drawStatsScreen();
}
