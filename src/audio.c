// No clue whats going on here

#include <SDL3/SDL.h>

#include "audio.h"

#define SAMPLE_RATE 44100
#define BEEP_FREQ_HZ 441

static SDL_AudioStream *stream = nullptr;
static SDL_AtomicInt beeping;
static double phase = 0.0;

static void SDLCALL audio_callback(void *userdata, SDL_AudioStream *astream,
                                    int additional_amount, int total_amount)
{
    (void)userdata;
    (void)total_amount;

    constexpr double phase_inc = (double)BEEP_FREQ_HZ / (double)SAMPLE_RATE;
    const bool on = SDL_GetAtomicInt(&beeping) != 0;

    while (additional_amount > 0)
    {
        int16_t samples[512];
        int chunk_bytes = (int)sizeof(samples);
        if (chunk_bytes > additional_amount)
            chunk_bytes = additional_amount;

        int chunk_samples = chunk_bytes / (int)sizeof(int16_t);

        for (int i = 0; i < chunk_samples; i++)
        {
            if (on)
            {
                samples[i] = (phase < 0.5) ? 6000 : -6000;
                phase += phase_inc;
                if (phase >= 1.0)
                    phase -= 1.0;
            }
            else
            {
                samples[i] = 0;
            }
        }

        SDL_PutAudioStreamData(astream, samples, chunk_bytes);
        additional_amount -= chunk_bytes;
    }
}

bool audio_init()
{
    SDL_SetAtomicInt(&beeping, 0);

    SDL_AudioSpec spec = {0};
    spec.format = SDL_AUDIO_S16;
    spec.channels = 1;
    spec.freq = SAMPLE_RATE;

    stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                                        &spec, audio_callback, nullptr);
    if (!stream)
    {
        SDL_Log("Failed to open audio device: %s", SDL_GetError());
        return false;
    }

    SDL_ResumeAudioStreamDevice(stream);
    return true;
}

void audio_set_beep(const bool on)
{
    SDL_SetAtomicInt(&beeping, on ? 1 : 0);
}

void audio_cleanup()
{
    if (stream)
    {
        SDL_DestroyAudioStream(stream);
        stream = nullptr;
    }
}
