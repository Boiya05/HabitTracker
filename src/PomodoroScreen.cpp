#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

#include "PomodoroScreen.h"
#include "StatsScreen.h" // for drawThickRect
#include "AudioManager.h"

extern Adafruit_ST7789 tft;

// ---- Setup screen state ----

enum PomodoroField { FIELD_WORK = 0, FIELD_BREAK = 1, FIELD_SESSIONS = 2 };

static const int MIN_WORK_BREAK = 5;
static const int MAX_WORK_BREAK = 55;
static const int STEP_WORK_BREAK = 5;
static const int MIN_SESSIONS = 1;
static const int MAX_SESSIONS = 12;

static int workMinutes = 25;
static int breakMinutes = 5;
static int sessionCount = 4;
static PomodoroField focusedField = FIELD_WORK;

static const int fieldBoxX = 40;
static const int fieldBoxW = 240;
static const int fieldBoxH = 40;
static const int fieldSpacing = 55;
static const int fieldStartY = 60;

static int fieldBoxY(int index) {
  return fieldStartY + index * fieldSpacing;
}

static void drawFieldBorder(int index) {
  int y = fieldBoxY(index);
  uint16_t color = ((int)focusedField == index) ? ST77XX_CYAN : ST77XX_WHITE;
  drawThickRect(fieldBoxX, y, fieldBoxW, fieldBoxH, color, 2);
}

static void drawFieldLabel(int index, const char* label) {
  int y = fieldBoxY(index);
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(fieldBoxX + 6, y - 12);
  tft.print(label);
}

static void drawFieldValue(int index) {
  int y = fieldBoxY(index);

  tft.fillRect(fieldBoxX + 6, y + 8, fieldBoxW - 12, 24, ST77XX_BLACK);

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(fieldBoxX + 10, y + 12);

  switch (index) {
    case FIELD_WORK:
      tft.print(workMinutes);
      tft.print(" min");
      break;
    case FIELD_BREAK:
      tft.print(breakMinutes);
      tft.print(" min");
      break;
    case FIELD_SESSIONS:
      tft.print(sessionCount);
      break;
  }
}

void drawPomodoroSetupScreen() {
  focusedField = FIELD_WORK;

  tft.fillScreen(ST77XX_BLACK);

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(15, 12);
  tft.print("Pomodoro Setup");

  tft.drawLine(0, 40, 320, 40, ST77XX_WHITE);

  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(15, 220);
  tft.print("Turn: adjust   Click: next field   Press: start");

  drawFieldLabel(FIELD_WORK, "Work Time");
  drawFieldLabel(FIELD_BREAK, "Break Time");
  drawFieldLabel(FIELD_SESSIONS, "Sessions");

  for (int i = 0; i < 3; i++) {
    drawFieldBorder(i);
    drawFieldValue(i);
  }
}

void pomodoroSetupMove(int direction) {
  switch (focusedField) {
    case FIELD_WORK:
      workMinutes += direction * STEP_WORK_BREAK;
      if (workMinutes < MIN_WORK_BREAK) workMinutes = MIN_WORK_BREAK;
      if (workMinutes > MAX_WORK_BREAK) workMinutes = MAX_WORK_BREAK;
      drawFieldValue(FIELD_WORK);
      break;

    case FIELD_BREAK:
      breakMinutes += direction * STEP_WORK_BREAK;
      if (breakMinutes < MIN_WORK_BREAK) breakMinutes = MIN_WORK_BREAK;
      if (breakMinutes > MAX_WORK_BREAK) breakMinutes = MAX_WORK_BREAK;
      drawFieldValue(FIELD_BREAK);
      break;

    case FIELD_SESSIONS:
      sessionCount += direction;
      if (sessionCount < MIN_SESSIONS) sessionCount = MIN_SESSIONS;
      if (sessionCount > MAX_SESSIONS) sessionCount = MAX_SESSIONS;
      drawFieldValue(FIELD_SESSIONS);
      break;
  }
}

void pomodoroSetupToggleFocus() {
  int oldField = focusedField;
  focusedField = (PomodoroField)((focusedField + 1) % 3);

  drawFieldBorder(oldField);
  drawFieldBorder(focusedField);
}

// ---- Timer screen state ----

enum PomodoroPhase { PHASE_WORK, PHASE_BREAK };

static PomodoroPhase currentPhase;
static int currentSession;
static int totalSessions;
static unsigned long phaseDurationMs;
static unsigned long phaseStartMs;
static int lastDisplayedSeconds;
static int lastBarFilledWidth;
static bool pomodoroFinished;
static bool isPaused;
static unsigned long pauseStartMs;

static const int barX = 40;
static const int barY = 190;
static const int barW = 240;
static const int barH = 20;

static uint16_t phaseColor() {
  return (currentPhase == PHASE_WORK) ? ST77XX_ORANGE : ST77XX_GREEN;
}

static void drawPhaseHeader() {
  tft.fillRect(0, 45, 320, 70, ST77XX_BLACK);

  tft.setTextSize(3);
  tft.setTextColor(phaseColor());
  tft.setCursor(15, 50);
  tft.print(currentPhase == PHASE_WORK ? "WORK" : "BREAK");

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(15, 85);
  tft.print("Session ");
  tft.print(currentSession);
  tft.print(" / ");
  tft.print(totalSessions);
}

static void drawCountdownText(int remainingSeconds) {
  int mm = remainingSeconds / 60;
  int ss = remainingSeconds % 60;

  char buf[6];
  snprintf(buf, sizeof(buf), "%02d:%02d", mm, ss);

  // Tight clear over the fixed-width monospace text instead of the whole screen.
  tft.fillRect(60, 115, 200, 48, ST77XX_BLACK);

  tft.setTextSize(6);
  tft.setTextColor(phaseColor());
  tft.setCursor(60, 115);
  tft.print(buf);
}

static void drawPausedIndicator() {
  tft.fillRect(0, 166, 320, 18, ST77XX_BLACK);

  if (isPaused) {
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_YELLOW);
    tft.setCursor(110, 166);
    tft.print("PAUSED");
  }
}

static void drawProgressBarOutline() {
  drawThickRect(barX - 2, barY - 2, barW + 4, barH + 4, ST77XX_WHITE, 2);
  lastBarFilledWidth = -1; // force the next fill to paint the full bar
}

static void updateProgressBar(long remainingMs, unsigned long totalMs) {
  int filledWidth = (int)((long)barW * remainingMs / (long)totalMs);
  if (filledWidth < 0) filledWidth = 0;
  if (filledWidth > barW) filledWidth = barW;

  if (lastBarFilledWidth < 0) {
    tft.fillRect(barX, barY, filledWidth, barH, phaseColor());
  } else if (filledWidth < lastBarFilledWidth) {
    // Only wipe the strip that drained since the last update.
    tft.fillRect(barX + filledWidth, barY, lastBarFilledWidth - filledWidth, barH, ST77XX_BLACK);
  }

  lastBarFilledWidth = filledWidth;
}

static void drawPomodoroCompleteScreen() {
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(15, 12);
  tft.print("Pomodoro");

  tft.drawLine(0, 40, 320, 40, ST77XX_WHITE);

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_GREEN);
  tft.setCursor(30, 100);
  tft.print("All sessions complete!");

  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(15, 220);
  tft.print("Press Back to return to setup");
}

static void beginPhase(PomodoroPhase phase, int minutes) {
  currentPhase = phase;
  phaseDurationMs = (unsigned long)minutes * 60UL * 1000UL;
  phaseStartMs = millis();
  lastDisplayedSeconds = -1;

  drawPhaseHeader();
  drawProgressBarOutline();
}

static void advancePomodoroPhase() {
  if (currentPhase == PHASE_WORK) {
    playTone(1500, 200); // work session done, break starting
    beginPhase(PHASE_BREAK, breakMinutes);
    return;
  }

  if (currentSession >= totalSessions) {
    // Distinct rising three-note chime so "fully done" doesn't sound like
    // just another phase change.
    playTone(1200, 150);
    delay(80);
    playTone(1600, 150);
    delay(80);
    playTone(2000, 250);

    pomodoroFinished = true;
    drawPomodoroCompleteScreen();
    return;
  }

  playTone(1000, 200); // break done, next work session starting
  currentSession++;
  beginPhase(PHASE_WORK, workMinutes);
}

void startPomodoroTimer() {
  totalSessions = sessionCount;
  currentSession = 1;
  pomodoroFinished = false;
  isPaused = false;

  tft.fillScreen(ST77XX_BLACK);

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(15, 12);
  tft.print("Pomodoro");

  tft.drawLine(0, 40, 320, 40, ST77XX_WHITE);

  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(15, 222);
  tft.print("Action: pause/resume   Back: cancel");

  beginPhase(PHASE_WORK, workMinutes);
  updatePomodoroTimer();
}

void togglePomodoroPause() {
  if (pomodoroFinished) return;

  if (isPaused) {
    // Resume: shift phaseStartMs forward by however long we sat paused, so the
    // remaining time picks up exactly where it left off instead of jumping.
    phaseStartMs += millis() - pauseStartMs;
    isPaused = false;
  } else {
    isPaused = true;
    pauseStartMs = millis();
  }

  drawPausedIndicator();
}

void updatePomodoroTimer() {
  if (pomodoroFinished || isPaused) return;

  unsigned long elapsed = millis() - phaseStartMs;
  long remainingMs = (long)phaseDurationMs - (long)elapsed;

  if (remainingMs <= 0) {
    advancePomodoroPhase();
    return;
  }

  int remainingSeconds = remainingMs / 1000;

  if (remainingSeconds != lastDisplayedSeconds) {
    lastDisplayedSeconds = remainingSeconds;
    drawCountdownText(remainingSeconds);
    updateProgressBar(remainingMs, phaseDurationMs);
  }
}
