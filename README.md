# Habit Tracker

# Hardware

## ESP32
Board: ESP32 Dev Module

## Display
Driver: ST7789V
Resolution: 240x320

### Wiring

| TFT  | ESP32 |
|------|--------|
| VCC  | 3.3V |
| GND  | GND |
| CS   | GPIO15 |
| DC   | GPIO2 |
| RST  | GPIO4 |
| MOSI | GPIO23 |
| SCK  | GPIO18 |
| MISO | GPIO19 |

### Notes
- Use Adafruit_ST7789
- tft.init(240,320)
- tft.invertDisplay(false)
