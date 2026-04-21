//INCLUDE's
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//DEF's
#define window_X  300 //Window X Axis Size 
#define window_Y  400 //Window Y Axis Size
#define thing_S   10  //Player Size
#define const_U   5   //integer  % const_U == 0
#define const_L   7   //integer  % const_L == 0
#define const_D   11  //integer  % const_D == 0
#define const_R   13  //integer  % const_R == 0
#define const_K   17  //integer  % const_K == 0
#define speed_X   thing_S  //Movement count X
#define speed_Y   thing_S  //Movement count Y
#define count_R   5		   //Possible Re-Do Count
#define r_P 	  0		   //Player Red
#define g_P 	  0		   //Player Green
#define b_P 	  0		   //Player Blue

//STRUCT's
typedef struct
{
    int     type; //Player == +1; Box = 0; Bomb = -1;
    int color[3]; //Block Color as R, G, B
    int   pos[2]; //Block Position as X, Y
    int  redo[count_R]; //Last Four Movement as Up = 5, Left = 7, Down = 11, Right = 13
} thing;

typedef struct
{
    SDL_Window   *window;
    SDL_Renderer *renderer;
    int running;
} sdl2;

//FUNCTION's - Draw
static void D_Grid (SDL_Renderer * renderer, int h, int w);               //Window, Target   X, Target   Y  
static void D_Thing(SDL_Renderer * renderer, thing * object); //Window, Position X, Position Y, Object to Draw
static void D_Color(thing * object); //Object to Color

//FUNCTION's - Input
int I_Handle(SDL_Event * event, thing * object);           //Input, Write Key

//FUNCTION's - Movement
static int  M_Bind (const int key); //Mod Check Key
static void M_Thing(thing * object, const int key); //Window, Object to Move, Read Key
static int  M_Bind_Redo(thing * object); //Object to Shift Right
static void M_Bind_Log (thing * object, const int key); //Object to Shift Left, Read Key
static void M_Thing_Redo(thing * object, const int key); //Object to Move, Read Key

//FUNCTION's - Physics
int P_Wall(thing object, const int go); //Object to Check, Axis
int P_Coll(thing object, const int go); //Object to Check, Axis

//FUNCTION's - Start
int S_SDL(sdl2 *a); //App 

//FUNCTION's - Main
int main(int argc, char * argv[]) //1.exe arg1, arg2 ... argn (n = argc, argn = argv[n])
{   
    sdl2 sukuban = {NULL, NULL, 0};

    switch (S_SDL(&sukuban))
    {
        case 1: printf(" \nvideo start failed"); return 0; break;
        case 2: printf("\nwindow start failed"); return 0; break;
        case 3: printf("\nrender start failed"); return 0; break;
        default: break;
    }
    SDL_Event event;
    int key_press;
    thing player =
    {
		.type = 1,
		.color = {r_P, g_P, b_P},
		.pos  = {140, 190},
		.redo = {-1,-1,-1,-1,-1}
	};
    D_Grid(sukuban.renderer, window_X, window_Y);
    while(sukuban.running)
    {
        while (SDL_PollEvent(&event))
        {

        if ( event.type == SDL_QUIT ) sukuban.running = 0;

        if (event.type == SDL_KEYDOWN)
        {
			key_press = I_Handle(&event, &player);
			
			if (key_press) M_Thing(&player, key_press);
		}	
		
		SDL_SetRenderDrawColor(sukuban.renderer, 0, 0, 0, 255);
		SDL_RenderClear(sukuban.renderer);

		D_Grid(sukuban.renderer, window_X, window_Y);
		D_Color(&player);
		D_Thing(sukuban.renderer, &player);

		SDL_RenderPresent(sukuban.renderer);
		
        }
        
    }
    return 0;
}

//FUNCTION 1 - Draw Grid
static void D_Grid (SDL_Renderer * renderer, int w, int h)
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	
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

    a->running = 1;
    return 0;
}

//FUNCTION 3 - Input Checker With Proper Key-bindings
int I_Handle(SDL_Event * event, thing * object)
{
    switch (event->key.keysym.sym)
        {
            case SDLK_w:
                M_Bind_Log(object, const_U);  // Up
                return 5;
            case SDLK_a:
                M_Bind_Log(object, const_L);  // Left
                return 7;
            case SDLK_s:
                M_Bind_Log(object, const_D);  // Down
                return 11;
            case SDLK_d:
                M_Bind_Log(object, const_R);  // Right
                return 13;
            case SDLK_u:
            {
                int last = M_Bind_Redo(object);  // Redo
                if (last){M_Thing_Redo(object, last);}
                return 17;
            }
            default:
                return 0;
        }
}

//FUNCTION 4 - Log Last count_R Input for Redo Function
static void M_Bind_Log(thing * object, const int key)
{
    for ( int i = count_R - 1 ; i > 0 ; i-- )
        object->redo[i] = object->redo[i - 1];
    object->redo[0] = key;
}

//FUNCTION 5 - Redo Last count_R Movement
static int M_Bind_Redo(thing * object)
{
	int buf = object->redo[0];
	if( buf == -1 ) return 0;
	for ( int i = 0 ; i < count_R - 1 ; i++ )
	{
		object->redo[i] = object->redo[i+1];
	}
	object->redo[count_R - 1] = -1;
	return buf;
}

//FUNCTION 6 - Draw Object Based On Type
static void D_Thing(SDL_Renderer * renderer, thing * object)
{
	SDL_SetRenderDrawColor(renderer, object->color[0], object->color[1], object->color[2], 255);
	SDL_Rect rect = {object->pos[0], object->pos[1], thing_S, thing_S};
	SDL_RenderFillRect(renderer, &rect);
}

//FUNCTION 7 - Color Object Based on Type
static void D_Color(thing * object)
{
	int r , g , b ;
	switch (object->type)
	{
		case  1:
			r = 255; g = 255; b = 255; break;
		case  0:
			r = 0  ; g = 255; b =   0; break;
		case -1:
			r = 255; g =   0; b =   0; break;
	};
	object->color[0] = r;
	object->color[1] = g;
	object->color[2] = b;
}

//FUNCTION 8 - Move Thing Without Collision Detection - Only For Test Purposes
static void M_Thing(thing * object, const int key)
{
	switch (key)
	{
		case const_U: object->pos[1] -= speed_Y; break;
		case const_D: object->pos[1] += speed_Y; break;
		case const_L: object->pos[0] -= speed_X; break;
		case const_R: object->pos[0] += speed_X; break; 
	}
}

//FUNCTION 9 - Move Thing as Redo 
static void M_Thing_Redo(thing * object, const int key)
{
	switch (key)
	{
		case const_U: object->pos[1] += speed_Y; break;
		case const_D: object->pos[1] -= speed_Y; break;
		case const_L: object->pos[0] += speed_X; break;
		case const_R: object->pos[0] -= speed_X; break; 
	}
}
