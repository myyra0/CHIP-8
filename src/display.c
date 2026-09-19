#include <SDL3/SDL.h>

#include "display.h"

bool display_init(Display *d)
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return false;
    }

    d->window = SDL_CreateWindow("CHIP 8", 1280, 640, SDL_WINDOW_RESIZABLE);
    if (!d->window)
    {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        return false;
    }

    d->renderer = SDL_CreateRenderer(d->window, nullptr);
    if (!d->renderer)
    {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        return false;
    }

    if (!SDL_SetRenderLogicalPresentation(d->renderer, 64, 32, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE))
    {
        SDL_Log("SDL_SetRenderLogicalPresentation failed: %s", SDL_GetError());
        return false;
    }

    d->texture = SDL_CreateTexture(d->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
    if (!d->texture)
    {
        SDL_Log("SDL_CreateTexture failed: %s", SDL_GetError());
        return false;
    }

    if (!SDL_SetTextureScaleMode(d->texture, SDL_SCALEMODE_NEAREST))
    {
        SDL_Log("SDL_SetTextureScaleMode failed: %s", SDL_GetError());
        return false;
    }

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

    if (!SDL_UpdateTexture(d->texture, nullptr, pixels, 64 * sizeof(uint32_t)))
    {
        SDL_Log("SDL_UpdateTexture failed: %s", SDL_GetError());
    }

    if (!SDL_RenderClear(d->renderer))
    {
        SDL_Log("SDL_RenderClear failed: %s", SDL_GetError());
    }

    if (!SDL_RenderTexture(d->renderer, d->texture, nullptr, nullptr))
    {
        SDL_Log("SDL_RenderTexture failed: %s", SDL_GetError());
    }

    if (!SDL_RenderPresent(d->renderer))
    {
        SDL_Log("SDL_RenderPresent failed: %s", SDL_GetError());
    }
}

void display_cleanup(Display *d)
{
    if (d->texture)
        SDL_DestroyTexture(d->texture);

    if (d->renderer)
        SDL_DestroyRenderer(d->renderer);

    if (d->window)
        SDL_DestroyWindow(d->window);

    SDL_Quit();
}
