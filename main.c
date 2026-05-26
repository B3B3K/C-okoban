//INCLUDE's
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

//DEF's
#define window_X    960 //Window X Axis Size 
#define window_Y    480 //Window Y Axis Size
#define thing_S     80  //Player Size (Grid Cell)
#define const_U     5   //integer  % const_U == 0
#define const_L     7   //integer  % const_L == 0
#define const_D     11  //integer  % const_D == 0
#define const_R     13  //integer  % const_R == 0
#define const_K     17  //integer  % const_K == 0
#define speed_X     1   //Movement count X (pixel per tick)
#define speed_Y     1   //Movement count Y (pixel per tick)
#define count_R     5   //Possible Re-Do Count
#define r_P         0   //Player Red
#define g_P         0   //Player Green
#define b_P         0   //Player Blue

#define FRAME_COUNT     4   //Frame Count For Player
#define FRAME_STEP      20  //Pixels moved before frame change
#define MOVE_STEPS      80  //Total pixels per one grid move (4 frames x 20px)
#define SPRITE_SIZE     16  //Frame Size (original PNG)

//STRUCT's
typedef struct
{
    int x;
    int y;
    int type;
} statics;
statics static_map[200]; // Maksimum 200 wall
int wall_count = 0; int box_count = 0;

typedef struct
{
    int     type;        //Player == +1; Box = 0; Bomb = -1; Wall = -2;
    int  color[3];       //Block Color as R, G, B
    int    pos[3];       //Block Position as X, Y, D
    int   redo[count_R]; //Last Movements: Up=5, Left=7, Down=11, Right=13
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
    SDL_Texture *frames[FRAME_COUNT]; //Frame textures
    int          current;             //Active frame index
    int          move_accum;          //Pixels moved since last frame change
} sprite;

typedef struct
{
    int active;    //Is movement in progress: 1 = yes, 0 = no
    int dir;       //Direction key (const_U/L/D/R), +100 offset for redo
    int remaining; //Remaining pixels to move this step
} motion;

thing wall = {.type = -2, .color = {255, 0, 0}, .pos = {0, 0, 0}, .redo = {-1,-1,-1,-1,-1}};


//FUNCTION's - Draw
static void D_Grid  (SDL_Renderer *renderer, int w, int h);              //Renderer, Width, Height
static void D_Thing (SDL_Renderer *renderer, thing *object, sprite *spr);//Renderer, Object, Sprite
static void D_Color (thing *object);                                      //Object to Color
void D_Static(SDL_Renderer *renderer, thing *object, SDL_Texture *texture); //Draw Static Elements With Sprite

//FUNCTION's - Sprite
int  SP_Load (sprite *spr, SDL_Renderer *renderer, int type); //Sprite, Renderer -> load 1.png/2.png/3.png & box
void SP_Step (sprite *spr, int pixels);             //Sprite, Pixels moved -> advance frame if needed
void SP_Free (sprite *spr);                         //Sprite -> free textures

//FUNCTION's - Motion
void MO_Start(motion *mo, int dir);                          //Motion, Direction -> begin smooth move
void MO_Tick (motion *mo, thing *object, sprite *spr);       //Motion, Object, Sprite -> advance one tick

//FUNCTION's - Input
int  I_Handle(SDL_Event *event, thing *object, motion *mo, thing *boxes);  //Event, Object, Motion -> return key

//FUNCTION's - Movement
static void M_Thing     (thing *object, const int key);      //Object, Key -> move 1px + wall check
static int  M_Bind_Redo (thing *object);                     //Object -> pop last redo key
static void M_Bind_Log  (thing *object, const int key);      //Object, Key -> push to redo log
static void M_Thing_Redo(thing *object, const int key);      //Object, Key -> reverse move 1px

//FUNCTION's - Physics
int P_Wall(thing *object);           //Object -> wall check, bounce back if hit
int P_Coll(thing object, int dir, thing *box); //Object, Axis, Box -> collision check (reserved)
int P_Rand(int key);                 //Key -> XOR random number
thing* P_Map(int map, SDL_Renderer *renderer); //Box Collector

//FUNCTION's - Start
int S_SDL(sdl2 *a);                  //App -> init SDL2, window, renderer

//FUNCTION's - Main
int main(int argc, char *argv[])
{
    sdl2 sukuban = {NULL, NULL, 0};

    switch (S_SDL(&sukuban))
    {
        case 1: printf(" \nvideo start failed"); return 0; break;
        case 2: printf("\nwindow start failed"); return 0; break;
        case 3: printf("\nrender start failed"); return 0; break;
        default: break;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        printf("\nIMG_Init failed: %s", IMG_GetError());
        return 0;
    }

    sprite player_sprite;
    sprite box_sprite;
    if (!SP_Load(&player_sprite, sukuban.renderer, 1))
    {
        printf("\nSprite yuklenemedi!");
        IMG_Quit();
        return 0;
    }
    if (!SP_Load(&box_sprite, sukuban.renderer, 0))
    {
        printf("\nSprite yuklenemedi!");
        IMG_Quit();
        return 0;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    SDL_Event event;
    thing player =
    {
        .type  = 1,
        .color = {r_P, g_P, b_P},
        .pos   = {80, 160, 1},
        .redo  = {-1,-1,-1,-1,-1}
    };

    motion move = {0, 0, 0};
    thing *boxes = NULL;

    while (sukuban.running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) sukuban.running = 0;

            if (event.type == SDL_KEYDOWN && !move.active)
                I_Handle(&event, &player, &move, boxes);
        }

        MO_Tick(&move, &player, &player_sprite);

        SDL_SetRenderDrawColor(sukuban.renderer, 0, 0, 0, 255);
        SDL_RenderClear(sukuban.renderer);
        if (sukuban.background) SDL_RenderCopy(sukuban.renderer, sukuban.background, NULL, NULL);
        D_Grid(sukuban.renderer, window_X, window_Y);
        D_Color(&player);
        boxes = P_Map(0, sukuban.renderer);
        for (int i = 0; i < box_count; i++)
        {
            D_Thing(sukuban.renderer, &boxes[i], &box_sprite);
        }
        D_Thing(sukuban.renderer, &player, &player_sprite);
        SDL_RenderPresent(sukuban.renderer);

        SDL_Delay(16);
    }

    SP_Free(&player_sprite);
    SP_Free(&box_sprite);
    IMG_Quit();
    return 0;
}

//FUNCTION 1 - Draw Grid
static void D_Grid(SDL_Renderer *renderer, int w, int h)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int x = 0; x < w; x += thing_S)
        SDL_RenderDrawLine(renderer, x, 0, x, h);
    for (int y = 0; y < h; y += thing_S)
        SDL_RenderDrawLine(renderer, 0, y, w, y);
}

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
    if (!a->background) {
        printf("\nError: %s", IMG_GetError());
    }

    a->running = 1;
    return 0;
}

//FUNCTION 3 - Input Handler
int I_Handle(SDL_Event *event, thing *object, motion *mo, thing *boxes)
{
    int key = 0;
    switch (event->key.keysym.sym)
    {
        case SDLK_w: key = const_U; break;
        case SDLK_a: key = const_L; break;
        case SDLK_s: key = const_D; break;
        case SDLK_d: key = const_R; break;
        case SDLK_u:
        {
            int last = M_Bind_Redo(object);
            if (last)
            {
                for (int i = 0; i < box_count; i++)
                {
                    int box_last = M_Bind_Redo(&boxes[i]);
                    if (box_last)
                    {
                        switch (box_last)
                        {
                            case const_U: boxes[i].pos[1] += thing_S; break;
                            case const_D: boxes[i].pos[1] -= thing_S; break;
                            case const_L: boxes[i].pos[0] += thing_S; break;
                            case const_R: boxes[i].pos[0] -= thing_S; break;
                        }
                    }
                }
                MO_Start(mo, last + 100);
            }
            return const_K;
        }
        default: return 0;
    }
    if (key)
    {
        int old_x[200], old_y[200];
        for (int i = 0; i < box_count; i++) {
            old_x[i] = boxes[i].pos[0];
            old_y[i] = boxes[i].pos[1];
        }
        if (!P_Coll(*object, key, boxes)) 
        {
            M_Bind_Log(object, key);
            MO_Start(mo, key);
            for (int i = 0; i < box_count; i++)
                if (boxes[i].pos[0] == old_x[i] && boxes[i].pos[1] == old_y[i])
                    M_Bind_Log(&boxes[i], 0);
        }
    }
    return key;
}

//FUNCTION 4 - Log Last count_R Input for Redo
static void M_Bind_Log(thing *object, const int key)
{
    for (int i = count_R - 1; i > 0; i--)
        object->redo[i] = object->redo[i - 1];
    object->redo[0] = key;
}

//FUNCTION 5 - Pop Last Redo
static int M_Bind_Redo(thing *object)
{
    int buf = object->redo[0];
    if (buf == -1) return 0;
    for (int i = 0; i < count_R - 1; i++)
        object->redo[i] = object->redo[i + 1];
    object->redo[count_R - 1] = -1;
    return buf;
}

//FUNCTION 6 - Draw Object
static void D_Thing(SDL_Renderer *renderer, thing *object, sprite *spr)
{
    SDL_Rect rect = {object->pos[0], object->pos[1], thing_S, thing_S};

    if (spr && spr->frames[spr->current])
    {
        //SDL_RenderCopy(renderer, spr->frames[spr->current], NULL, &rect); BUG!!!
        double angle = 0.0;
        switch (object->pos[2])
        {
        case const_U: angle = 0.0; break;
        case const_D: angle = 180.0; break;
        case const_L: angle = 270.0; break;
        case const_R: angle = 90.0; break;
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
static void D_Color(thing *object)
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

//FUNCTION 8 - Redo
static void M_Thing_Redo(thing *object, const int key)
{
    switch (key)
    {
        case const_U: object->pos[1] += speed_Y; break;
        case const_D: object->pos[1] -= speed_Y; break;
        case const_L: object->pos[0] += speed_X; break;
        case const_R: object->pos[0] -= speed_X; break;
    }
}

//FUNCTION 9 - Wall Check
int P_Wall(thing *object)
{
    if (object->pos[0] > window_X - thing_S) object->pos[0] = window_X - thing_S;
    if (object->pos[0] < 0)                  object->pos[0] = 0;
    if (object->pos[1] > window_Y - thing_S) object->pos[1] = window_Y - thing_S;
    if (object->pos[1] < 0)                  object->pos[1] = 0;
    
    return 0;
}

//FUNCTION 10 - Move Object With Wall Check
static void M_Thing(thing *object, const int key)
{
    switch (key)
    {
        case const_U: object->pos[1] -= speed_Y; object->pos[2] = const_U; break;
        case const_D: object->pos[1] += speed_Y; object->pos[2] = const_D; break;
        case const_L: object->pos[0] -= speed_X; object->pos[2] = const_L; break;
        case const_R: object->pos[0] += speed_X; object->pos[2] = const_R; break;
    }
}

//FUNCTION 11 - XOR Random Number Generator
static unsigned int XOR_Random_Generator(int key_scancode)
{
    static unsigned int state = 0;
    static int first_call = 1;
    if (first_call) { state = (unsigned int)time(NULL); first_call = 0; }
    state ^= (unsigned int)key_scancode;
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
}

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
            printf("\nError: %s\nl", files[i], IMG_GetError());
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

//FUNCTION 13 - Butter Movement Per Frame
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

//FUNCTION 15 - Butter Movement Starter
void MO_Start(motion *mo, int dir)
{
    mo->active    = 1;
    mo->dir       = dir;
    mo->remaining = MOVE_STEPS;
}

//FUNCTION 16 - Butter Movement Main Loop
void MO_Tick(motion *mo, thing *object, sprite *spr)
{
    if (!mo->active) return;

    int is_redo = (mo->dir > 100);
    int dir     = is_redo ? mo->dir - 100 : mo->dir;

    if (is_redo)
        M_Thing_Redo(object, dir);
    else
        M_Thing(object, dir);

    SP_Step(spr, 1);

    mo->remaining--;
    if (mo->remaining <= 0)
    {
        mo->active = 0;
        object->pos[0] = (object->pos[0] / thing_S) * thing_S;
        object->pos[1] = (object->pos[1] / thing_S) * thing_S;
    }
}

//FUNCTION 17 - Map Place
thing* P_Map(int map, SDL_Renderer *renderer)
{
    static char filename[20];
    static SDL_Texture *tree_texture = NULL;
    static SDL_Texture *bomb_texture = NULL;
    static int loaded_map = -1;
    static thing *boxes = NULL;

    if (tree_texture == NULL) {
        tree_texture = IMG_LoadTexture(renderer, "A.png");
        bomb_texture = IMG_LoadTexture(renderer, "Z.png");
        if (!tree_texture || !bomb_texture) {
            printf("\nTexture loading failed: %s", IMG_GetError());
            return NULL;
        }
    }

    if (loaded_map != map)
    {
        wall_count = 0;
        box_count  = 0;
        loaded_map = map;

        sprintf(filename, "%d.txt", map);
        FILE *f = fopen(filename, "r");
        if (!f) {
            printf("\nCannot open file: %s", filename);
            return NULL;
        }
        
        int ch;
        while ((ch = fgetc(f)) != EOF)
            if (ch == 'o') box_count++;
        rewind(f);
        
        if (boxes) free(boxes);
        boxes = malloc(box_count * sizeof(thing));
        if (!boxes) { fclose(f); return NULL; }

        int x = 0, y = 0, box_index = 0;
        while ((ch = fgetc(f)) != EOF && y < (window_Y / thing_S))
        {
            if (ch == 'x' || ch == '!')
            {
                static_map[wall_count].x    = x * thing_S;
                static_map[wall_count].y    = y * thing_S;
                static_map[wall_count].type = ch;
                wall_count++;
            }
            else if (ch == 'o')
            {
                boxes[box_index].type   = 0;
                boxes[box_index].pos[0] = x * thing_S;
                boxes[box_index].pos[1] = y * thing_S;
                boxes[box_index].pos[2] = 1;
                for (int i = 0; i < count_R; i++) boxes[box_index].redo[i] = -1;
                box_index++;
            }
            else if (ch == '\n') { y++; x = 0; continue; }
            x++;
        }
        fclose(f);
    }

    for (int i = 0; i < wall_count; i++)
    {
        SDL_Rect rect = {static_map[i].x, static_map[i].y, thing_S, thing_S};
        SDL_Texture *tex = (static_map[i].type == 'x') ? tree_texture : bomb_texture;
        SDL_RenderCopy(renderer, tex, NULL, &rect);
    }

    return boxes;
}

//FUCNTION 18 - Static Drawings With Sprite
void D_Static(SDL_Renderer *renderer, thing *object, SDL_Texture *texture)
{
    SDL_Rect rect = {object->pos[0], object->pos[1], thing_S, thing_S};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}

//FUNCTION 19 - Collision Check from array
int P_Coll(thing object, int dir, thing *box)
{
    int nx = object.pos[0];
    int ny = object.pos[1];

    switch (dir)
    {
        case const_U: ny -= thing_S; break;
        case const_D: ny += thing_S; break;
        case const_L: nx -= thing_S; break;
        case const_R: nx += thing_S; break;
        default: return 0;
    }

    for (int i = 0; i < wall_count; i++)
        if (nx == static_map[i].x && ny == static_map[i].y)
            return 1;

    for (int i = 0; i < box_count; i++)
    if (box[i].pos[0] == nx && box[i].pos[1] == ny)
    {
        if (P_Coll(box[i], dir, box))
            return 1;
            
        switch (dir)
        {
            case const_U: box[i].pos[1] -= thing_S; break;
            case const_D: box[i].pos[1] += thing_S; break;
            case const_L: box[i].pos[0] -= thing_S; break;
            case const_R: box[i].pos[0] += thing_S; break;
        }
        M_Bind_Log(&box[i], dir);
        return 0;
    }
    return 0;
}

//FUNCTION 20 - Push Box Check
