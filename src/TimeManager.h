#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include <Arduino.h>
#include <time.h>

void setupTime();
void updateTime();

void setUtcOffsetHours(int hours);
int getUtcOffsetHours();
void applyUtcOffset();

int getHour();
int getMinute();
int getSecond();

int getDay();
int getMonth();
int getYear();

int getDaysInMonth(int year, int month);
int getFirstWeekdayOfMonth(int year, int month);

String getWeekday();
String getMonthName();
String getMonthNameFor(int month);
String getDateKey();

#endif