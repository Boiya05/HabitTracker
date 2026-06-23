#include <Arduino.h>
#include <Adafruit_GFX.h> //talks to screen driver
#include <Adafruit_ST7789.h> //gives drawing function
#include <SPI.h>

#include "Config.h"
#include "Display.h"

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);


void setupDisplay() {
  tft.init(240, 320); // set resolution
  tft.invertDisplay(false);
  tft.setRotation(1); // set rotation
  tft.fillScreen(ST77XX_BLACK); //fills screen black
}

void drawMainMenu(int selectedItem, const char* menuItems[], int menuCount) {
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextSize(3);
  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(20, 15);  //set position of text
  tft.println("Habit Box");

  tft.drawLine(0, 55, 320, 55, ST77XX_WHITE);

  tft.setTextSize(2);

  for (int i = 0; i < menuCount; i++) {
    int y = 80 + i * 35;

    if (i == selectedItem) {
      tft.fillRect(10, y - 5, 300, 28, ST77XX_BLUE);
      tft.setTextColor(ST77XX_WHITE);
      tft.setCursor(25, y);
      tft.print("> ");
      tft.println(menuItems[i]);
    } else {
      tft.setTextColor(ST77XX_WHITE);
      tft.setCursor(25, y);
      tft.print("  ");
      tft.println(menuItems[i]);
    }
  }
}

void drawSelectedScreen(const char* title) {
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextSize(3);
  tft.setTextColor(ST77XX_GREEN);
  tft.setCursor(20, 40);
  tft.println(title);

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(20, 110);
  tft.println("Screen opened.");

  tft.setCursor(20, 150);
  tft.println("Press to go back.");
}