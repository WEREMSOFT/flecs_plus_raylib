#include "raylib.h"
#ifdef PI

#undef PI
#endif
#define SP_DRAW_DOUBLE_FACED

#define SP_LAYER_SPACING 0.5

#define SP_LAYER_SPACING_BASE -1.0

#include <spine-raylib.h>
#include <spine/spine.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif
#include "flecs.h"

typedef struct GameContext {
    Vector2 screen_size;
    Camera3D camera;
} GameContext;

GameContext game_context;

typedef struct SPAsset {
    spAtlas* atlas;
    spSkeletonJson* json;
    spSkeletonData* skeletonData;
    spSkeleton* skeleton;
    spAnimationStateData* animationStateData;
    spAnimationState* animationState;
} SPAsset;

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450

GameContext init_game_context(){
    printf("Initializando\n");

    GameContext return_value = {0};

    return_value.screen_size.x = SCREEN_WIDTH;
    return_value.screen_size.y = SCREEN_HEIGHT;

    // Define the camera to look into our 3d world
    return_value.camera.position = (Vector3){ 10.0f, 10.0f, 10.0f }; // Camera position
    return_value.camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    return_value.camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    return_value.camera.fovy = 45.0f;                                // Camera field-of-view Y
    return_value.camera.type = CAMERA_PERSPECTIVE;                   // Camera mode type

    SetCameraMode(return_value.camera, CAMERA_ORBITAL); // Set a free camera mode

    return return_value;
}

void cube_init_position(ecs_rows_t *rows){
    ECS_COLUMN(rows, Vector3, position, 1);

    for(int i = 0; i < rows->count; i++){
        position[i].x = position[i].y = position->z = 0;
    }
}

ecs_world_t *world;



void pre_render(GameContext* game_context){
    BeginDrawing();
    ClearBackground(RAYWHITE);
    BeginMode3D(game_context->camera);
    UpdateCamera(&game_context->camera);
}

void post_render(){
    DrawGrid(10, 1.0f);
    EndMode3D();
    DrawFPS(10, 10);
    EndDrawing();
}

void game_update() {
    pre_render(&game_context);
    ecs_progress(world, 0);
    post_render();
}

void render(ecs_rows_t *rows){
    ECS_COLUMN(rows, Vector3, position, 1);

    rows->delta_time

    for (int i = 0; i < rows->count; i ++) {
        DrawCube(position[i], 2.0f, 2.0f, 2.0f, CLITERAL(Color){11, 110, 176, 255});
        DrawCubeWires(position[i], 2.0f, 2.0f, 2.0f, MAROON);
    }
}

int main(int argc, char* argv[]) {
    world = ecs_init_w_args(argc, argv);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib [core] example - 3d camera free");
    SetTargetFPS(60);

    game_context = init_game_context();

    ECS_COMPONENT(world, Vector3);
    ECS_COMPONENT(world, SPAsset);
    ECS_TAG(world, Redereable);

    ECS_SYSTEM(world, render, EcsOnUpdate, Vector3, Redereable);
    ECS_SYSTEM(world, cube_init_position, EcsOnAdd, Vector3, Redereable);
    ecs_set_system_context(world, render, &game_context);

    ECS_ENTITY(world, cube, Vector3, Redereable);
    ECS_ENTITY(world, dragon, Vector3, SPAsset);

    ecs_set_target_fps(world, 60);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(game_update, 0, 1);
#else

    while(!WindowShouldClose()){
        pre_render(&game_context);
        ecs_progress(world, 0);
        post_render();
    }

    ecs_fini(world);

#endif
}