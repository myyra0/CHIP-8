#ifndef CHIP_8_AUDIO_H
#define CHIP_8_AUDIO_H

bool audio_init();

void audio_set_beep(bool on);

void audio_cleanup();

#endif //CHIP_8_AUDIO_H
