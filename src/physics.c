#include "physics.h"

// Global map state definitions
statics static_map[200];
int wall_count = 0;
int box_count  = 0;

//FUNCTION 4 - Log Last count_R Input for Redo
void M_Bind_Log(thing *object, const int key)
{
    for (int i = count_R - 1; i > 0; i--)
        object->redo[i] = object->redo[i - 1];
    object->redo[0] = key;
}

//FUNCTION 5 - Pop Last Redo
int M_Bind_Redo(thing *object)
{
    int buf = object->redo[0];
    if (buf == -1) return 0;
    for (int i = 0; i < count_R - 1; i++)
        object->redo[i] = object->redo[i + 1];
    object->redo[count_R - 1] = -1;
    return buf;
}

//FUNCTION 9 - Wall Boundary Check
int P_Wall(thing *object)
{
    if (object->pos[0] > window_X - thing_S) object->pos[0] = window_X - thing_S;
    if (object->pos[0] < 0)                  object->pos[0] = 0;
    if (object->pos[1] > window_Y - thing_S) object->pos[1] = window_Y - thing_S;
    if (object->pos[1] < 0)                  object->pos[1] = 0;
    return 0;
}

//FUNCTION 11 - XOR Random Number Generator
static unsigned int XOR_Random_Generator(int key_scancode)
{
    static unsigned int state    = 0;
    static int          first_call = 1;
    if (first_call) { state = (unsigned int)time(NULL); first_call = 0; }
    state ^= (unsigned int)key_scancode;
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
}

int P_Rand(int key)
{
    return (int)(XOR_Random_Generator(key) % 4);
}

//FUNCTION 19 - Collision Check From Array
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
        if (nx == static_map[i].x && ny == static_map[i].y && static_map[i].type == 'x')
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

//FUNCTION 17 - Map Loader
thing* P_Map(int map, SDL_Renderer *renderer)
{
    static char        filename[20];
    static SDL_Texture *tree_texture = NULL;
    static SDL_Texture *bomb_texture = NULL;
    static int          loaded_map   = -1;
    static thing       *boxes        = NULL;

    if (tree_texture == NULL)
    {
        tree_texture = IMG_LoadTexture(renderer, "A.png");
        bomb_texture = IMG_LoadTexture(renderer, "Z.png");
        if (!tree_texture || !bomb_texture)
        {
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
        if (!f) { printf("\nCannot open file: %s", filename); return NULL; }

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
        SDL_Rect     rect = {static_map[i].x, static_map[i].y, thing_S, thing_S};
        SDL_Texture *tex  = (static_map[i].type == 'x') ? tree_texture : bomb_texture;
        SDL_RenderCopy(renderer, tex, NULL, &rect);
    }

    return boxes;
}

//FUNCTION 20 - Win Condition Check
int P_Check_Win(thing *boxes)
{
    if (!boxes) return 0;

    int bomb_count = 0;
    for (int i = 0; i < wall_count; i++)
    {
        if (static_map[i].type == '!')
        {
            bomb_count++;
            int covered = 0;
            for (int j = 0; j < box_count; j++)
            {
                if (boxes[j].pos[0] == static_map[i].x && boxes[j].pos[1] == static_map[i].y)
                {
                    covered = 1;
                    break;
                }
            }
            if (!covered) return 0;
        }
    }
    return (bomb_count > 0) ? 1 : 0;
}
