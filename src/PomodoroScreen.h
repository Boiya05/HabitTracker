#ifndef POMODORO_SCREEN_H
#define POMODORO_SCREEN_H

// Setup screen: pick work/break minutes and session count
void drawPomodoroSetupScreen();
void pomodoroSetupMove(int direction);
void pomodoroSetupToggleFocus();

// Timer screen: countdown + progress bar, driven by the chosen setup values
void startPomodoroTimer();
void updatePomodoroTimer();
void togglePomodoroPause();

#endif
