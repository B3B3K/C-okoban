#ifndef MOTION_H
#define MOTION_H

#include "types.h"
#include "sprite.h"

void MO_Start(motion *mo, int dir);

void MO_Tick (motion *mo, thing *object, sprite *spr, thing *boxes);

#endif 
