#include "motion.h"
#include "physics.h"  // M_Bind_Redo
#include "movement.h" // M_Thing, M_Thing_Redo

//FUNCTION 15 - Start Smooth Movement
void MO_Start(motion *mo, int dir)
{
    mo->active    = 1;
    mo->dir       = dir;
    mo->remaining = MOVE_STEPS;
}

//FUNCTION 16 - Smooth Movement Main Loop
void MO_Tick(motion *mo, thing *object, sprite *spr, thing *boxes)
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

        if (is_redo && boxes)
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
        }
    }
}
