#include "types.h"
#include "s_sdl.h"
#include "draw.h"
#include "sprite.h"
#include "motion.h"
#include "input.h"
#include "physics.h"

int main(int argc, char *argv[])
{
    sdl2 sukuban = {NULL, NULL, NULL, 0};

    switch (S_SDL(&sukuban))
    {
        case 1: printf("\nvideo start failed");  return 0;
        case 2: printf("\nwindow start failed"); return 0;
        case 3: printf("\nrender start failed"); return 0;
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
        printf("\nPlayer sprite error!");
        IMG_Quit();
        return 0;
    }
    if (!SP_Load(&box_sprite, sukuban.renderer, 0))
    {
        printf("\nBox sprite error!");
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
        .redo  = {-1, -1, -1, -1, -1}
    };

    motion move        = {0, 0, 0};
    thing *boxes       = NULL;
    int    current_map = 0;
    int    total_steps = 0;

    while (sukuban.running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) sukuban.running = 0;

            if (event.type == SDL_KEYDOWN && !move.active)
            {
                I_Handle(&event, &player, &move, boxes);
                if (move.active) total_steps++;
            }
        }

        MO_Tick(&move, &player, &player_sprite, boxes);

        SDL_SetRenderDrawColor(sukuban.renderer, 0, 0, 0, 255);
        SDL_RenderClear(sukuban.renderer);
        if (sukuban.background)
            SDL_RenderCopy(sukuban.renderer, sukuban.background, NULL, NULL);

        D_Grid(sukuban.renderer, window_X, window_Y);
        D_Color(&player);

        boxes = P_Map(current_map, sukuban.renderer);

        if (!boxes && current_map > 0)
        {
            SDL_SetWindowTitle(sukuban.window, "YOU WIN!");
            SDL_Texture *win_tex = IMG_LoadTexture(sukuban.renderer, "w.png");
            while (sukuban.running)
            {
                while (SDL_PollEvent(&event))
                    if (event.type == SDL_QUIT) sukuban.running = 0;
                SDL_RenderCopy(sukuban.renderer, win_tex, NULL, NULL);
                SDL_RenderPresent(sukuban.renderer);
            }
            break;
        }

        for (int i = 0; i < box_count; i++)
            D_Thing(sukuban.renderer, &boxes[i], &box_sprite);

        D_Thing(sukuban.renderer, &player, &player_sprite);
        SDL_RenderPresent(sukuban.renderer);

        int valid_redos = 0;
        for (int i = 0; i < count_R; i++)
            if (player.redo[i] != -1) valid_redos++;

        char title_text[128];
        sprintf(title_text, "C-okoban | Level: %d | Adim: %d | Redo: %d ", current_map + 1, total_steps, valid_redos);
        SDL_SetWindowTitle(sukuban.window, title_text);

        if (!move.active && P_Check_Win(boxes))
        {
            printf("%d fin\n", current_map);
            char next_filename[20];
            sprintf(next_filename, "%d.txt", current_map + 1);
            FILE *check = fopen(next_filename, "r");
            
            if (check)
            {
                fclose(check);
                D_Show_Next_Level(sukuban.renderer, sukuban.window, current_map + 1, &box_sprite, &player, boxes);
                if (!sukuban.running) break;
                current_map++;
                P_Reset_Player(&player);
            }
            else
            {
                D_Show_Game_End(sukuban.renderer, sukuban.window, &sukuban.running);
                break;
            }
            
            total_steps = 0;
            for (int i = 0; i < count_R; i++) player.redo[i] = -1;
        }

        SDL_Delay(16);
    }

    SP_Free(&player_sprite);
    SP_Free(&box_sprite);
    IMG_Quit();
    return 0;
}
