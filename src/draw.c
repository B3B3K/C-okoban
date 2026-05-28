#include "draw.h"
#include "physics.h"

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

//FUNCTION 22 - Show Next Level Screen
void D_Show_Next_Level(SDL_Renderer *renderer, SDL_Window *window, int next_map, sprite *box_sprite, thing *player, thing *old_boxes)
{
    SDL_Event event;
    SDL_Texture *next_tex = IMG_LoadTexture(renderer, "N.png");
    int show_next = 1;
    
    while (show_next)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                show_next = -1;
                break;
            }
            if (event.type == SDL_KEYDOWN)
                show_next = 0;
        }
        
        if (show_next == -1) break;
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, NULL, NULL, NULL);
        
        D_Grid(renderer, window_X, window_Y);
        D_Color(player);
        
        thing *next_boxes = P_Map(next_map, renderer);
        if (next_boxes)
        {
            for (int i = 0; i < box_count; i++)
                D_Thing(renderer, &next_boxes[i], box_sprite);
        }
        
        thing temp_player = *player;
        P_Reset_Player(&temp_player);
        D_Thing(renderer, &temp_player, NULL);
        
        if (next_tex)
            SDL_RenderCopy(renderer, next_tex, NULL, NULL);
        
        SDL_RenderPresent(renderer);
    }
    
    if (next_tex)
        SDL_DestroyTexture(next_tex);
}

//FUNCTION 23 - Show Game End Screen
void D_Show_Game_End(SDL_Renderer *renderer, SDL_Window *window, int *running)
{
    SDL_Event event;
    SDL_Texture *win_tex = IMG_LoadTexture(renderer, "w.png");
    
    SDL_SetWindowTitle(window, "YOU WIN!");
    
    while (*running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                *running = 0;
        }
        
        if (win_tex)
            SDL_RenderCopy(renderer, win_tex, NULL, NULL);
        
        SDL_RenderPresent(renderer);
    }
    
    if (win_tex)
        SDL_DestroyTexture(win_tex);
}
