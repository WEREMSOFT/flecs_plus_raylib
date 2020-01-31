//
// Created by Pablo Weremczuk on 1/31/20.
//

#ifndef RAYLIBTEST_GAME_H
#define RAYLIBTEST_GAME_H

#include <math.h>

void cube_init_position(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, position, 1);

    for (int i = 0; i < rows->count; i++) {
        position[i].x = position[i].y = position[i].z = 0;
    }
}

void sp_asset_init(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, position, 1);
    ECS_COLUMN(rows, sp_asset_t, sp_assets, 2);

    sp_asset_t *dragon = ecs_get_system_context(rows->world, rows->system);

    float phase = PI2 / rows->count;

    for (int i = 0; i < rows->count; i++) {
        position[i].x = 400 + sin(i * phase) * 100;
        position[i].y = 200 + cos(i * phase) * 100;
        position[i].z = 0;
        sp_assets[i] = *dragon;
    }
}


void pre_render(ecs_rows_t *rows) {
    GameContext *game_context = ecs_get_system_context(rows->world, rows->system);

    UpdateCamera(&game_context->camera);
    BeginDrawing();

    ClearBackground(RAYWHITE);
    BeginMode3D(game_context->camera);
    DrawGrid(100, 100.0f);
}

void post_render(ecs_rows_t *rows) {
    DrawFPS(10, 10);
    if (GuiButton((Rectangle) {500, 20, 140, 30}, "Open Image")) { printf("otro click!!\n"); }
    if (GuiButton((Rectangle) {20, 50, 140, 30}, GuiIconText(RICON_FILE_OPEN, "Open Image"))) printf("boton!!\n");

    GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
    //GuiSetStyle(VALUEBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);

    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);

    GuiGroupBox((Rectangle) {25, 110, 125, 150}, "STATES");
    GuiSetState(GUI_STATE_NORMAL);
    if (GuiButton((Rectangle) {30, 120, 115, 30}, "NORMAL")) {}
    GuiSetState(GUI_STATE_FOCUSED);
    if (GuiButton((Rectangle) {30, 155, 115, 30}, "FOCUSED")) {}
    GuiSetState(GUI_STATE_PRESSED);
    if (GuiButton((Rectangle) {30, 190, 115, 30}, "#15#PRESSED")) {}
    GuiSetState(GUI_STATE_DISABLED);
    if (GuiButton((Rectangle) {30, 225, 115, 30}, "DISABLED")) {}
    GuiSetState(GUI_STATE_NORMAL);

    GuiComboBox((Rectangle) {25, 470, 125, 30}, "ONE;TWO;THREE;FOUR", NULL);

    EndDrawing();
}

void render(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, position, 1);

    for (int i = 0; i < rows->count; i++) {
        DrawCube(position[i], 100.0f, 100.0f, 100.0f, CLITERAL(Color) {11, 110, 176, 255});
        DrawCubeWires(position[i], 100.0f, 100.0f, 100.0f, MAROON);
    }
}

void render_sp_assets(ecs_rows_t *rows) {
    EndMode3D();
    ECS_COLUMN(rows, Vector3, position, 1);
    ECS_COLUMN(rows, sp_asset_t, assets, 2);
    spAnimationState_update(assets[0].animationState, GetFrameTime());
    spAnimationState_apply(assets[0].animationState, assets[0].skeleton);
    spSkeleton_updateWorldTransform(assets[0].skeleton);

    for (int i = 0; i < rows->count; i++) {
        drawSkeleton(assets[i].skeleton, position[i]);
    }
}

void init_game_world(ecs_world_t *world, GameContext* game_context){
    ECS_COMPONENT(world, Vector3);
    ECS_COMPONENT(world, sp_asset_t);
    ECS_TAG(world, Redereable);

    ECS_SYSTEM(world, pre_render, EcsOnUpdate, Redereable);
    ECS_SYSTEM(world, render, EcsOnUpdate, Vector3, Redereable);
    ECS_SYSTEM(world, render_sp_assets, EcsOnUpdate, Vector3, sp_asset_t);
    ECS_SYSTEM(world, post_render, EcsOnUpdate, Redereable);
    ECS_SYSTEM(world, cube_init_position, EcsOnAdd, Vector3, Redereable);
    ECS_SYSTEM(world, sp_asset_init, EcsOnAdd, Vector3, sp_asset_t);

    ecs_set_system_context(world, pre_render, game_context);
    ecs_set_system_context(world, sp_asset_init, &spine_assets[DRAGON]);

    ECS_ENTITY(world, cube, Vector3, Redereable);

    ECS_TYPE(world, Dragon, Vector3, sp_asset_t);
    ecs_new_w_count(world, Dragon, 6);

    ecs_set_target_fps(world, 60);
}

#endif //RAYLIBTEST_GAME_H
