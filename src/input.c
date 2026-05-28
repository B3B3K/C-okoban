#include "input.h"
#include "physics.h"   // P_Coll, M_Bind_Log, M_Bind_Redo

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
            if (last) MO_Start(mo, last + 100);
            return const_K;
        }
        default: return 0;
    }

    if (key)
    {
        int old_x[200], old_y[200];
        for (int i = 0; i < box_count; i++)
        {
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
