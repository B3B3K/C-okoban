#include "draw.h"

//FUNCTION 1 - Draw Grid
void D_Grid(SDL_Renderer *renderer, int w, int h)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int x = 0; x < w; x += thing_S)
        SDL_RenderDrawLine(renderer, x, 0, x, h);
    for (int y = 0; y < h; y += thing_S)
        SDL_RenderDrawLine(renderer, 0, y, w, y);
}

//FUNCTION 6 - Draw Object
void D_Thing(SDL_Renderer *renderer, thing *object, sprite *spr)
{
    SDL_Rect rect = {object->pos[0], object->pos[1], thing_S, thing_S};

    if (spr && spr->frames[spr->current])
    {
        double angle = 0.0;
        switch (object->pos[2])
        {
            case const_U: angle =   0.0; break;
            case const_D: angle = 180.0; break;
            case const_L: angle = 270.0; break;
            case const_R: angle =  90.0; break;
        }
        SDL_RenderCopyEx(renderer, spr->frames[spr->current], NULL, &rect, angle, NULL, SDL_FLIP_NONE);
    }
    else
    {
        SDL_SetRenderDrawColor(renderer, object->color[0], object->color[1], object->color[2], 255);
        SDL_RenderFillRect(renderer, &rect);
    }
}

//FUNCTION 7 - Color Object Based on Type
void D_Color(thing *object)
{
    int r, g, b;
    switch (object->type)
    {
        case  1: r = 255; g = 255; b = 255; break;
        case  0: r =   0; g = 255; b =   0; break;
        case -1: r = 255; g =   0; b =   0; break;
        default: r =   0; g =   0; b =   0; break;
    }
    object->color[0] = r;
    object->color[1] = g;
    object->color[2] = b;
}

//FUNCTION 18 - Static Drawing With Sprite
void D_Static(SDL_Renderer *renderer, thing *object, SDL_Texture *texture)
{
    SDL_Rect rect = {object->pos[0], object->pos[1], thing_S, thing_S};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}
