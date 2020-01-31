#include <pthread.h>
#include <unistd.h>
#include "raylib.h"

#ifdef PI
#undef PI
#endif
//#define SP_DRAW_DOUBLE_FACED

#define SP_LAYER_SPACING 0
#define SP_LAYER_SPACING_BASE 0
#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS

#include <spine-raylib.h>
#include "raygui.h"

#include <spine/spine.h>
#include "flecs.h"

#include "utils/structs.h"
#include "utils/common.h"
#include "utils/assets.h"
#include "screens/game.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
ecs_world_t *world;
void game_update() {
    ecs_progress(world, 0);
}
#endif

ecs_world_t* screens[SCREEN_COUNT] = {0};

GameContext game_context = {0};

int main(int argc, char *argv[]) {

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib [core] example - 3d camera free");
    init_assets();

    for(int i = 0; i < SCREEN_COUNT; i++){
        screens[i] = ecs_init_w_args(argc, argv);
    }


    SetTargetFPS(60);
    game_context = init_game_context();

     init_game_world(screens[SCREEN_GAME], &game_context);

     game_context.world = screens[SCREEN_GAME];

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(game_update, 0, 1);
#else
    while (ecs_progress(game_context.world, 0) && !WindowShouldClose());

    for(int i = 0; i < SCREEN_COUNT; i++){
        ecs_fini(screens[i]);
    }

    destroy_assets();
    CloseWindow();        // Close window and OpenGL context
#endif
}