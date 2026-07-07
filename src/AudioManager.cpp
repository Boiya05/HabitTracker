#include <Arduino.h>

#include "AudioManager.h"
#include "Config.h"

// A PAM8403 expects a line-level analog signal on L/R, but for simple alert
// tones a PWM square wave works fine — the board's own input coupling
// capacitor blocks the DC bias and just passes the AC tone through to the
// speaker. SPEAKER_PIN feeds both the L and R inputs together (see Config.h).
static const int SPEAKER_CHANNEL = 0;
static const int TONE_RESOLUTION_BITS = 10; // duty range 0-1023

// ledcWriteTone() always drives a fixed ~50% duty square wave, which gives no
// way to control loudness. Driving the timer/duty ourselves lets a lower
// duty cycle (below the 50% "max amplitude" point) approximate volume — a
// crude technique, but effective enough for short alert beeps.
static int volumePercent = 70;

void setupAudio() {
  ledcSetup(SPEAKER_CHANNEL, 2000, TONE_RESOLUTION_BITS);
  ledcAttachPin(SPEAKER_PIN, SPEAKER_CHANNEL);
  ledcWrite(SPEAKER_CHANNEL, 0); // silent until playTone() is called
}

void setVolumePercent(int percent) {
  if (percent < 0) percent = 0;
  if (percent > 100) percent = 100;
  volumePercent = percent;
}

int getVolumePercent() {
  return volumePercent;
}

void playTone(unsigned int frequencyHz, unsigned int durationMs) {
  if (volumePercent <= 0 || frequencyHz == 0) {
    delay(durationMs); // keep the same timing/feel even when muted
    return;
  }

  ledcChangeFrequency(SPEAKER_CHANNEL, frequencyHz, TONE_RESOLUTION_BITS);

  // A square wave's loudness is dominated by its fundamental's amplitude,
  // which is proportional to sin(pi * dutyRatio) — a curve that's nearly
  // flat near its 50%-duty peak. A plain linear percent-to-duty mapping
  // therefore spends most of the 0-100% range sitting on that flat part
  // (barely any audible change) and crams all the real loudness change into
  // the bottom of the range. Squaring the normalized fraction first pulls
  // low settings down much faster and spreads the audible change out across
  // the whole dial instead — the same reason real volume knobs use a "log
  // taper" rather than a linear one.
  float normalized = volumePercent / 100.0f;
  float tapered = normalized * normalized;

  uint32_t maxDuty = 1 << (TONE_RESOLUTION_BITS - 1); // 50% duty = max square-wave amplitude
  uint32_t duty = (uint32_t)(maxDuty * tapered);

  ledcWrite(SPEAKER_CHANNEL, duty);
  delay(durationMs);
  ledcWrite(SPEAKER_CHANNEL, 0);
}
