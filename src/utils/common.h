//
// Created by Pablo Weremczuk on 1/31/20.
//

#ifndef RAYLIBTEST_COMMON_H
#define RAYLIBTEST_COMMON_H

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 800

GameContext init_game_context() {
    printf("Initializando\n");

    GameContext return_value = {0};

    return_value.screen_size.x = SCREEN_WIDTH;
    return_value.screen_size.y = SCREEN_HEIGHT;

    // Define the camera to look into our 3d world
    return_value.camera.position = (Vector3) {300.0f, -300.0f, 300.0f};
    return_value.camera.target = (Vector3) {0.0f, 0.0f, 0.0f};      // Camera looking at point
    return_value.camera.up = (Vector3) {0.0f, -1.0f, 0.0f};          // Camera up vector (rotation towards target)
    return_value.camera.fovy = 45.0f;                                // Camera field-of-view Y
    return_value.camera.type = CAMERA_PERSPECTIVE;                   // Camera mode type

    SetCameraMode(return_value.camera, CAMERA_ORBITAL); // Set a free camera mode

    return return_value;
}

#endif //RAYLIBTEST_COMMON_H
