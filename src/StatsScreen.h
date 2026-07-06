#ifndef STATS_SCREEN_H
#define STATS_SCREEN_H

#include <Arduino.h>

void drawStatsScreen();
void drawDate();
void drawDay();
void drawCalendarUI();
void drawDAYselectBOX(int day, bool selected);
void drawSelectedDayInfo();
void statsMoveDay(int direction);
void drawThickRect(int x, int y, int w, int h, uint16_t color, int thickness);
int getSelectedDay();
void drawDayDetailScreen(int day);
#endif