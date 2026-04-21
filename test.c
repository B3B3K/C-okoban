//INCLUDE's
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//DEF's
#define window_X  300 //Window X Axis Size 
#define window_Y  400 //Window Y Axis Size
#define thing_S   10 //Player Size
#define const_U   5   //integer  % const_U == 0
#define const_L   7   //integer  % const_L == 0
#define const_D   11  //integer  % const_D == 0
#define const_R   13  //integer  % const_R == 0
#define speed_X   thing_S  //Movement count X
#define speed_Y   thing_S  //Movement count Y

//STRUCT's
typedef struct
{
    int     type; //Block Type
    int color[3]; //Block Color as R, G, B
    int   pos[2]; //Block Position as X, Y
    int  redo[4]; //Last Four Movement as Up = 5, Left = 7, Down = 11, Right = 13
} thing;

typedef struct
{
    int wasdu[5]; //Movement as Up, Left, Down, Right, Redo
} movement;

typedef struct
{
    SDL_Window   *window;
    SDL_Renderer *renderer;
} sdl2;

//FUNCTION's - Draw
static void D_Grid (SDL_Renderer * renderer, int h, int w);               //Window, Target   X, Target   Y  
static void D_Thing(SDL_Renderer * renderer, int x, int y, thing object); //Window, Position X, Position Y, Object to Draw

//FUNCTION's - Input
int I_Handle(SDL_Event * event, movement * keyboard);             //Input, Write Key

//FUNCTION's - Movement
static int  M_Bind (const int key); //Mod Check Key
static void M_Thing(SDL_Renderer * renderer, thing object, const int key); //Window, Object to Move, Read Key
static void M_Bind_Redo(thing * object); //Object to Shift Right
static void M_Bind_Log (thing * object, const int key); //Object to Shift Left, Read Key

//FUNCTION's - Physics
int P_Wall(thing object, const int go); //Object to Check, Axis
int P_Coll(thing object, const int go); //Object to Check, Axis

//FUNCTION's - Start
int S_SDL(sdl2 *a); //App 

//FUNCTION's - Main
int main()
{   
    sdl2 sukuban = {NULL, NULL};
    switch (S_SDL(&sukuban))
    {
        case 1: printf(" \nvideo start failed"); return 0; break;
        case 2: printf("\nwindow start failed"); return 0; break;
        case 3: printf("\nrender start failed"); return 0; break;
        default: break;
    }
    return 0;
}

//FUNCTION 1 - Draw Grid
static void D_Grid (SDL_Renderer * renderer, int h, int w)
{
    for(int x = 0; x < w; x += thing_S)
        SDL_RenderDrawLine(renderer, x, 0, x, h);
    for(int y = 0; y < h; y += thing_S)
        SDL_RenderDrawLine(renderer, 0, y, w, y);
}

//FUNCTION 2 - Start SDL2 with Proper Window Outline
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

    return 0;
}