#include <Arduino.h>
#include "Config.h"
#include "Input.h"

int lastCLK;
bool lastButtonState = HIGH;

void setupInput() {
  pinMode(ENC_CLK, INPUT_PULLUP);
  pinMode(ENC_DT, INPUT_PULLUP);
  pinMode(ENC_SW, INPUT_PULLUP);
  pinMode(BTN_BACK, INPUT_PULLUP);
  pinMode(BTN_ACTION, INPUT_PULLUP);

  lastCLK = digitalRead(ENC_CLK);
}

int getEncoderMove() {
  int currentCLK = digitalRead(ENC_CLK);

  if (currentCLK != lastCLK && currentCLK == LOW) {
    lastCLK = currentCLK;

    if (digitalRead(ENC_DT) != currentCLK) {
      return 1;   // clockwise
    } else {
      return -1;  // counter-clockwise
    }
  }

  lastCLK = currentCLK;
  return 0;
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
    delay(150);
    return true;
  }

  lastState = currentState;
  return false;
}