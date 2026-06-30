#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include <Arduino.h>
#include <time.h>

void setupTime();
void updateTime();

int getHour();
int getMinute();
int getSecond();

int getDay();
int getMonth();
int getYear();

String getWeekday();
String getMonthName();
String getDateKey();

#endif