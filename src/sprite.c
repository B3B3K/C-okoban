#include "sprite.h"

//FUNCTION 12 - Load Sprite Frames
int SP_Load(sprite *spr, SDL_Renderer *renderer, int type)
{
    const char *files[FRAME_COUNT];

    const char *p_files[FRAME_COUNT] = {"1.png", "2.png", "1.png", "3.png"};
    const char *b_files[FRAME_COUNT] = {"k.png", "k.png", "k.png", "k.png"};

    memcpy(files, (type == 1) ? p_files : b_files, sizeof(files));

    spr->current    = 0;
    spr->move_accum = 0;

    for (int i = 0; i < FRAME_COUNT; i++)
    {
        SDL_Surface *surf = IMG_Load(files[i]);
        if (!surf)
        {
            printf("\nError: %s\n", files[i]);
            for (int j = 0; j < i; j++) SDL_DestroyTexture(spr->frames[j]);
            return 0;
        }
        spr->frames[i] = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
        if (!spr->frames[i])
        {
            printf("\nError: %s\n", SDL_GetError());
            for (int j = 0; j < i; j++) SDL_DestroyTexture(spr->frames[j]);
            return 0;
        }
    }
    return 1;
}

//FUNCTION 13 - Advance Frame Per Movement
void SP_Step(sprite *spr, int pixels)
{
    spr->move_accum += pixels;
    if (spr->move_accum >= FRAME_STEP)
    {
        spr->current    = (spr->current + 1) % FRAME_COUNT;
        spr->move_accum = 0;
    }
}

//FUNCTION 14 - Free Sprite Textures
void SP_Free(sprite *spr)
{
    for (int i = 0; i < FRAME_COUNT; i++)
        if (spr->frames[i]) SDL_DestroyTexture(spr->frames[i]);
}
