#include <SDL3/SDL.h>

#include "display.h"

bool display_init(Display *d)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);

    d->window = SDL_CreateWindow("CHIP 8", 1280, 640, SDL_WINDOW_RESIZABLE);

    d->renderer = SDL_CreateRenderer(d->window, nullptr);

    SDL_SetRenderLogicalPresentation(d->renderer, 64, 32, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

    d->texture = SDL_CreateTexture(d->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

    SDL_SetTextureScaleMode(d->texture, SDL_SCALEMODE_NEAREST);

    return true;
}

void display_render(Display *d, const bool emu_display[64][32])
{
    // Converting into a format SDL recognizes and uses
    uint32_t pixels[64 * 32];

    for (int y = 0; y < 32; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            // White if active, opaque black if inactive
            pixels[y * 64 + x] = emu_display[x][y] ? 0xFFFFFFFF : 0x000000FF;
        }
    }

    SDL_UpdateTexture(d->texture, nullptr, pixels, 64 * sizeof(uint32_t));

    SDL_RenderClear(d->renderer);
    SDL_RenderTexture(d->renderer, d->texture, nullptr, nullptr);
    SDL_RenderPresent(d->renderer);
}

void display_cleanup(Display *d)
{
    if (d->texture)
        SDL_DestroyTexture(d->texture);

    SDL_DestroyRenderer(d->renderer);

    SDL_DestroyWindow(d->window);

    SDL_Quit();
}
