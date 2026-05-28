#include "s_sdl.h"

//FUNCTION 2 - Start SDL2
int S_SDL(sdl2 *a)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) return 1;

    a->window = SDL_CreateWindow("250229017",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        window_X, window_Y, SDL_WINDOW_SHOWN);
    if (!a->window) return 2;

    a->renderer = SDL_CreateRenderer(a->window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!a->renderer) return 3;

    a->background = IMG_LoadTexture(a->renderer, "b.png");
    if (!a->background)
        printf("\nBackground texture error: %s", IMG_GetError());

    a->running = 1;
    return 0;
}
