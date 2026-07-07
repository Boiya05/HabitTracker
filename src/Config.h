#ifndef CONFIG_H
#define CONFIG_H

// TFT pins
#define TFT_CS   5
#define TFT_DC   16
#define TFT_RST  17

// TFT backlight (PWM-dimmable). Requires wiring the display module's
// backlight/LED pin to this GPIO instead of straight to VCC — most breakout
// boards ship with that pin hardwired to VCC by default, so check yours.
#define TFT_BL   13

// Rotary encoder pins
#define ENC_CLK 25
#define ENC_DT  26
#define ENC_SW  27

// Button pins
#define BTN_BACK    32
#define BTN_ACTION  33

// Audio (PAM8403 amplifier). One PWM pin feeds both the L and R inputs —
// these are alert tones, not stereo audio, so both channels play the same
// signal. GPIO21/22 are left free for a future I2C RTC (SDA/SCL).
#define SPEAKER_PIN 4

//Number of habits
const int HABIT_COUNT = 4;

// Single source of truth for habit names — defined in HabitTrackerScreen.cpp,
// used by both HabitTrackerScreen and StatsScreen so they can't drift apart.
extern const char* habitNames[HABIT_COUNT];



#endif