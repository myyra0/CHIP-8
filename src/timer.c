#include "main.h"
#include "audio.h"

#include "timer.h"

void decrement_delaytimer(Chip8 *chip8)
{
    if (chip8->delay_timer > 0)
        chip8->delay_timer--;
}

void decrement_soundtimer(Chip8 *chip8)
{
    if (chip8->sound_timer > 0)
        chip8->sound_timer--;

    audio_set_beep(chip8->sound_timer > 0);
}
