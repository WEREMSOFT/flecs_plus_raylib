#include "raylib.h"
#ifdef PI

#undef PI
#endif
#define SP_DRAW_DOUBLE_FACED

#define SP_LAYER_SPACING 0.1
#define SP_LAYER_SPACING_BASE -0.1

#include <spine-raylib.h>
#include <spine/spine.h>
#include <math.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif
#include "flecs.h"

typedef struct GameContext {
    Vector2 screen_size;
    Camera3D camera;
} GameContext;

typedef struct sp_asset_t {
    spAtlas* atlas;
    spSkeletonJson* json;
    spSkeletonData* skeletonData;
    spSkeleton* skeleton;
    spAnimationStateData* animationStateData;
    spAnimationState* animationState;
} sp_asset_t;

sp_asset_t sp_asset_create(){

    sp_asset_t return_value = {0};

    // Init spine
    return_value.atlas = spAtlas_createFromFile("assets/dragon/NewDragon.atlas", 0);
    return_value.json = spSkeletonJson_create(return_value.atlas);

    return_value.skeletonData = spSkeletonJson_readSkeletonDataFile(return_value.json, "assets/dragon/NewDragon.json");

    if (!return_value.skeletonData) {
        printf("%s\n", return_value.json->error);
        spSkeletonJson_dispose(return_value.json);
        printf("ERROR!\n");
    }

    spBone_setYDown(true);
    return_value.skeleton = spSkeleton_create(return_value.skeletonData);
    return_value.skeleton->scaleX = 0.1;
    return_value.skeleton->scaleY = 0.1;

    // Create the spAnimationStateData
    return_value.animationStateData = spAnimationStateData_create(return_value.skeletonData);
    return_value.animationState = spAnimationState_create(return_value.animationStateData);
    // Add the animation "walk" to track 0, without delay, and let it loop indefinitely
    int track = 0;
    int loop = 1;
    float delay = 0;
    spAnimationState_addAnimationByName(return_value.animationState, track, "flying", loop, delay);
    spAnimationState_addAnimationByName(return_value.animationState, 0, "flying", 1, 0);
    spAnimationState_update(return_value.animationState,.0f);
    spAnimationState_apply(return_value.animationState, return_value.skeleton);
    spSkeleton_updateWorldTransform(return_value.skeleton);

    return return_value;
}

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450

GameContext init_game_context(){
    printf("Initializando\n");

    GameContext return_value = {0};

    return_value.screen_size.x = SCREEN_WIDTH;
    return_value.screen_size.y = SCREEN_HEIGHT;

    // Define the camera to look into our 3d world
    return_value.camera.position = (Vector3){ 300.0f, -300.0f, 300.0f };
//    return_value.camera.position = (Vector3){ 10.0f, 10.0f, 10.0f }; // Camera position
    return_value.camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    return_value.camera.up = (Vector3){ 0.0f, -1.0f, 0.0f };          // Camera up vector (rotation towards target)
    return_value.camera.fovy = 45.0f;                                // Camera field-of-view Y
    return_value.camera.type = CAMERA_PERSPECTIVE;                   // Camera mode type

    SetCameraMode(return_value.camera, CAMERA_ORBITAL); // Set a free camera mode

    return return_value;
}

void cube_init_position(ecs_rows_t *rows){
    ECS_COLUMN(rows, Vector3, position, 1);

    for(int i = 0; i < rows->count; i++){
        position[i].x = position[i].y = position[i].z = 0;
    }
}

void sp_asset_init(ecs_rows_t *rows){
    ECS_COLUMN(rows, Vector3, position, 1);
    ECS_COLUMN(rows, sp_asset_t, sp_assets, 2);

    sp_asset_t *dragon = ecs_get_system_context(rows->world, rows->system);

    float phase = PI2 / rows->count;

    for(int i = 0; i < rows->count; i++){
        position[i].x = sin(i * phase) * 200;
        position[i].y = -100;
        position[i].z = cos(i * phase) * 200;;
        sp_assets[i] = *dragon;
    }
}

void pre_render(ecs_rows_t *rows){
    GameContext* game_context = ecs_get_system_context(rows->world, rows->system);

    UpdateCamera(&game_context->camera);
    BeginDrawing();

    ClearBackground(RAYWHITE);
    BeginMode3D(game_context->camera);
    DrawGrid(100, 100.0f);
}

void post_render(ecs_rows_t *rows){
    EndMode3D();
    DrawFPS(10, 10);
    EndDrawing();
}

void render(ecs_rows_t *rows){
    ECS_COLUMN(rows, Vector3, position, 1);

    for (int i = 0; i < rows->count; i ++) {
        DrawCube(position[i], 100.0f, 100.0f, 100.0f, CLITERAL(Color){11, 110, 176, 255});
        DrawCubeWires(position[i], 100.0f, 100.0f, 100.0f, MAROON);
    }
}

void render_sp_assets(ecs_rows_t *rows){
    ECS_COLUMN(rows, Vector3, position, 1);
    ECS_COLUMN(rows, sp_asset_t, assets, 2);
    spAnimationState_update(assets[0].animationState, GetFrameTime());
    spAnimationState_apply(assets[0].animationState, assets[0].skeleton);
    spSkeleton_updateWorldTransform(assets[0].skeleton);

    for (int i = 0; i < rows->count; i ++) {
        drawSkeleton(assets[i].skeleton, position[i]);
    }
}

#if defined(PLATFORM_WEB)
ecs_world_t *world;
void game_update() {
    ecs_progress(world, 0);
}
#endif

int main(int argc, char* argv[]) {
#if defined(PLATFORM_WEB)
    world = ecs_init_w_args(argc, argv);
#else
    ecs_world_t *world = ecs_init_w_args(argc, argv);
#endif
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib [core] example - 3d camera free");
    SetTargetFPS(60);
    GameContext game_context = init_game_context();

    ECS_COMPONENT(world, Vector3);
    ECS_COMPONENT(world, sp_asset_t);
    ECS_TAG(world, Redereable);

    ECS_SYSTEM(world, render, EcsOnUpdate, Vector3, Redereable);
    ECS_SYSTEM(world, pre_render, EcsPreUpdate, Redereable);
    ECS_SYSTEM(world, render_sp_assets, EcsPreUpdate, Vector3, sp_asset_t);
    ECS_SYSTEM(world, post_render, EcsPostUpdate, Redereable);
    ECS_SYSTEM(world, cube_init_position, EcsOnAdd, Vector3, Redereable);
    ECS_SYSTEM(world, sp_asset_init, EcsOnAdd, Vector3, sp_asset_t);

    sp_asset_t dragon = sp_asset_create();

    ecs_set_system_context(world, pre_render, &game_context);
    ecs_set_system_context(world, sp_asset_init, &dragon);

    ECS_ENTITY(world, cube, Vector3, Redereable);

    ECS_TYPE(world, Dragon, Vector3, sp_asset_t);
    ecs_new_w_count(world, Dragon, 500);

    ecs_set_target_fps(world, 60);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(game_update, 0, 1);
#else
    while(ecs_progress(world, 0) && !WindowShouldClose());
    ecs_fini(world);
    spAtlas_dispose(dragon.atlas);
    spSkeleton_dispose(dragon.skeleton);
    texture_2d_destroy(); // Destroy textures loaded by spine
    CloseWindow();        // Close window and OpenGL context
#endif
}