#ifndef TYPES_H
#define TYPES_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define window_X    960
#define window_Y    480
#define thing_S     80
#define const_U     5
#define const_L     7
#define const_D     11
#define const_R     13
#define const_K     17
#define speed_X     1
#define speed_Y     1
#define count_R     5
#define r_P         0
#define g_P         0
#define b_P         0

#define FRAME_COUNT     4
#define FRAME_STEP      20
#define MOVE_STEPS      80
#define SPRITE_SIZE     16

typedef struct
{
    int x;
    int y;
    int type;
} statics;

typedef struct
{
    int     type;
    int  color[3];
    int    pos[3];
    int   redo[count_R];
} thing;

typedef struct
{
    SDL_Window   *window;
    SDL_Renderer *renderer;
    SDL_Texture  *background;
    int running;
} sdl2;

typedef struct
{
    SDL_Texture *frames[FRAME_COUNT];
    int          current;
    int          move_accum;
} sprite;

typedef struct
{
    int active;
    int dir;
    int remaining;
} motion;

extern statics static_map[200];
extern int wall_count;
extern int box_count;

#endif
