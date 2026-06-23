#ifndef DISPLAY_H
#define DISPLAY_H

void setupDisplay();
void drawMainMenu(int selectedItem, const char* menuItems[], int menuCount);
void drawSelectedScreen(const char* title);

#endif