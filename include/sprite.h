#ifndef SPRITE_H
#define SPRITE_H

#include "types.h"

int  SP_Load(sprite *spr, SDL_Renderer *renderer, int type);

void SP_Step(sprite *spr, int pixels);

void SP_Free(sprite *spr);

#endif 
