#ifndef PHYSICS_H
#define PHYSICS_H

#include "types.h"

int    P_Wall     (thing *object);

int    P_Coll     (thing object, int dir, thing *box);

int    P_Rand     (int key);

thing* P_Map      (int map, SDL_Renderer *renderer);

int    P_Check_Win(thing *boxes);

void   M_Bind_Log (thing *object, const int key);

int    M_Bind_Redo(thing *object);

void   P_Reset_Player(thing *player);

#endif
