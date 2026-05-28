#include "movement.h"

//FUNCTION 10 - Move Object 1px With Direction Update
void M_Thing(thing *object, const int key)
{
    switch (key)
    {
        case const_U: object->pos[1] -= speed_Y; object->pos[2] = const_U; break;
        case const_D: object->pos[1] += speed_Y; object->pos[2] = const_D; break;
        case const_L: object->pos[0] -= speed_X; object->pos[2] = const_L; break;
        case const_R: object->pos[0] += speed_X; object->pos[2] = const_R; break;
    }
}

//FUNCTION 8 - Reverse Move 1px (Redo)
void M_Thing_Redo(thing *object, const int key)
{
    switch (key)
    {
        case const_U: object->pos[1] += speed_Y; break;
        case const_D: object->pos[1] -= speed_Y; break;
        case const_L: object->pos[0] += speed_X; break;
        case const_R: object->pos[0] -= speed_X; break;
    }
}
