#ifndef SETTINGS_SCREEN_H
#define SETTINGS_SCREEN_H

// Loads persisted settings at boot and applies them (brightness, volume,
// UTC offset) — call once from setup(), before setupTime().
void setupSettingsStorage();

void drawSettingsScreen();
void settingsMove(int direction);
void settingsToggleFocus();

// Persists the current live values and re-applies the UTC offset — call when
// leaving the Settings screen (Back).
void saveSettings();

#endif
