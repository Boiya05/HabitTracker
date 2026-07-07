#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

void setupAudio();
void playTone(unsigned int frequencyHz, unsigned int durationMs);

void setVolumePercent(int percent);
int getVolumePercent();

#endif
