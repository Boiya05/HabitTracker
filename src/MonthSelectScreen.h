#ifndef MONTH_SELECT_SCREEN_H
#define MONTH_SELECT_SCREEN_H

void drawMonthSelectScreen();
void redrawMonthSelectScreen();
void monthSelectMove(int direction);
void monthSelectToggleFocus();

int getMonthSelectMonth();
int getMonthSelectYear();

#endif
