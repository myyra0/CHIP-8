#ifndef CHIP_8_DISPLAY_H
#define CHIP_8_DISPLAY_H

#include <SDL3/SDL.h>

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} Display;

bool display_init(Display *d);

void display_render(Display *d, const bool emu_display[64][32]);

void display_cleanup(Display *d);

#endif // CHIP_8_DISPLAY_H
