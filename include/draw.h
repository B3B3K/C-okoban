#ifndef DRAW_H
#define DRAW_H

#include "types.h"

void D_Grid  (SDL_Renderer *renderer, int w, int h);

void D_Thing (SDL_Renderer *renderer, thing *object, sprite *spr);

void D_Color (thing *object);

void D_Static(SDL_Renderer *renderer, thing *object, SDL_Texture *texture);

#endif
