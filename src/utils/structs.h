//
// Created by Pablo Weremczuk on 1/31/20.
//

#ifndef RAYLIBTEST_STRUCTS_H
#define RAYLIBTEST_STRUCTS_H

enum Sreens {
    SCREEN_MAIN_MENU,
    SCREEN_TUTORIAL,
    SCREEN_GAME,
    SCREEN_COUNT
};

typedef struct GameContext {
    Vector2 screen_size;
    Camera3D camera;
    ecs_world_t *world;
} GameContext;

typedef struct sp_asset_t {
    spAtlas *atlas;
    spSkeletonJson *json;
    spSkeletonData *skeletonData;
    spSkeleton *skeleton;
    spAnimationStateData *animationStateData;
    spAnimationState *animationState;
} sp_asset_t;

#endif //RAYLIBTEST_STRUCTS_H
