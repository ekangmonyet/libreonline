#include <cmath>
#include "raylib.h"


int main()
{
    InitWindow(1280, 720, "libreONLINE");
    Camera camera {
        .position = {20.0f, 20.0f, 20.0f},
        .target = {0.0f, 0.0f, 0.0f},
        .up = {0.0f, 1.6f, 0.0f},
        .fovy = 45.0f,
        .projection = CAMERA_PERSPECTIVE,
    };

    Model player = LoadModel("assets/Mecha01.obj");
    Texture2D player_t = LoadTexture("assets/Mecha01.png");
    Vector3 player_m {};
    float player_r = 0;
    player.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = player_t;

    SetCameraMode(camera, CAMERA_THIRD_PERSON);
    SetTargetFPS(30);

    while (!WindowShouldClose()) {
        int forward = 0;
        if (IsKeyDown(KEY_W))
            forward = 1;
        if (IsKeyDown(KEY_S))
            forward = -1;
        if (IsKeyDown(KEY_A))
            player_r += 10;
        if (IsKeyDown(KEY_D))
            player_r -= 10;

        player_m.x += forward * 1.0f * std::cos(player_r * PI / 180);
        player_m.z -= forward * 1.0f * std::sin(player_r * PI / 180);

        camera.target = player_m;

        UpdateCamera(&camera);

        BeginDrawing();
        BeginMode3D(camera);
        ClearBackground(RAYWHITE);

        DrawModelEx(player, player_m, {0.0f, 1.0f, 0.0f}, player_r, {1.0f, 1.0f, 1.0f}, WHITE);

        DrawGrid(100, 1.0f);
        EndMode3D();
        DrawFPS(10, 10);
        EndDrawing();

    }

    UnloadModel(player);
    CloseWindow();

    return 0;
}
