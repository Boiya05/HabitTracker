#ifndef DISPLAY_H
#define DISPLAY_H

void setupDisplay();
void drawMenuItem(int i, bool selected, const char* menuItems[]);
void drawMainMenu(int selectedItem, const char* menuItems[], int menuCount);
void drawSelectedScreen(const char* title);
void drawMainMenuStatic();

#endif