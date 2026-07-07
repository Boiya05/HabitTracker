#include <Arduino.h>
#include "Config.h"
#include "Input.h"
#include "AudioManager.h"

// Input feedback tones. Kept short and distinct so the encoder "tick" doesn't
// slow down fast scrolling, while Action/Back get a more noticeable beep —
// those already pay a 150ms software-debounce delay per press, so a longer
// tone there doesn't cost any extra perceived latency.
static const unsigned int ENCODER_TICK_HZ   = 1500;
static const unsigned int ENCODER_TICK_MS   = 12;
static const unsigned int ACTION_BEEP_HZ    = 2500; // higher pitch
static const unsigned int ACTION_BEEP_MS    = 50;
static const unsigned int BACK_BEEP_HZ      = 600;  // lower pitch
static const unsigned int BACK_BEEP_MS      = 50;

// ---- Rotary encoder: interrupt-driven quadrature decode ----
//
// The old approach polled CLK once per loop() and guessed direction from a
// single read of DT. That has two failure modes on real hardware: contact
// bounce during a single detent can look like several separate falling
// edges (overscroll), and a transition that happens while loop() is busy
// elsewhere (mid-draw, inside a debounce delay(), etc.) is simply never
// seen (underscroll).
//
// Fix: decode the full 4-state Gray code sequence via a transition table,
// and drive it from interrupts on both pins so no transition is missed
// regardless of what the main loop is doing. Bounce naturally produces
// "invalid" state transitions (skipping a state, or reporting a state
// that isn't Gray-code-adjacent to the last one), which the table maps to
// zero instead of counting them.

// Set true if rotating the encoder moves the UI the opposite way you expect.
static const bool ENCODER_REVERSED = false;

// Valid Gray-code step table, indexed by (oldState << 2) | newState, where
// state = (CLK << 1) | DT. Adjacent valid states differ in exactly one bit;
// anything else is bounce/noise and contributes 0.
static const int8_t QUAD_TABLE[16] = {
   0, -1,  1,  0,
   1,  0,  0, -1,
  -1,  0,  0,  1,
   0,  1, -1,  0
};

static volatile uint8_t encoderState = 0;
static volatile int8_t  encoderStepAccumulator = 0;
static volatile long    encoderPosition = 0; // one count per full mechanical detent

static long lastConsumedPosition = 0;

void IRAM_ATTR handleEncoderInterrupt() {
  uint8_t clk = digitalRead(ENC_CLK);
  uint8_t dt  = digitalRead(ENC_DT);
  uint8_t newState = (clk << 1) | dt;

  int8_t delta = QUAD_TABLE[(encoderState << 2) | newState];
  encoderState = newState;

  if (delta == 0) return;

  encoderStepAccumulator += delta;

  // Most encoders produce a full 4-state Gray cycle per mechanical detent.
  if (encoderStepAccumulator >= 4) {
    encoderPosition++;
    encoderStepAccumulator = 0;
  } else if (encoderStepAccumulator <= -4) {
    encoderPosition--;
    encoderStepAccumulator = 0;
  }
}

bool lastButtonState = HIGH;

void setupInput() {
  pinMode(ENC_CLK, INPUT_PULLUP);
  pinMode(ENC_DT, INPUT_PULLUP);
  pinMode(ENC_SW, INPUT_PULLUP);
  pinMode(BTN_BACK, INPUT_PULLUP);
  pinMode(BTN_ACTION, INPUT_PULLUP);

  encoderState = (digitalRead(ENC_CLK) << 1) | digitalRead(ENC_DT);

  attachInterrupt(digitalPinToInterrupt(ENC_CLK), handleEncoderInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_DT), handleEncoderInterrupt, CHANGE);
}

int getEncoderMove() {
  long currentPosition = encoderPosition;

  int direction = 0;

  if (currentPosition > lastConsumedPosition) {
    lastConsumedPosition++;
    direction = 1;
  } else if (currentPosition < lastConsumedPosition) {
    lastConsumedPosition--;
    direction = -1;
  } else {
    return 0;
  }

  playTone(ENCODER_TICK_HZ, ENCODER_TICK_MS);

  return ENCODER_REVERSED ? -direction : direction;
}

bool isButtonPressed() {
  bool currentState = digitalRead(ENC_SW);

  if (currentState == LOW && lastButtonState == HIGH) {
    lastButtonState = currentState;
    delay(150);
    return true;
  }

  lastButtonState = currentState;
  return false;
}

bool isBackPressed() {

  static bool lastState = HIGH;

  bool currentState = digitalRead(BTN_BACK);

  if (currentState == LOW && lastState == HIGH) {
    lastState = currentState;
    playTone(BACK_BEEP_HZ, BACK_BEEP_MS);
    delay(150);
    return true;
  }

  lastState = currentState;
  return false;
}

bool isActionPressed() {

  static bool lastState = HIGH;

  bool currentState = digitalRead(BTN_ACTION);

  if (currentState == LOW && lastState == HIGH) {
    lastState = currentState;
    playTone(ACTION_BEEP_HZ, ACTION_BEEP_MS);
    delay(150);
    return true;
  }

  lastState = currentState;
  return false;
}
