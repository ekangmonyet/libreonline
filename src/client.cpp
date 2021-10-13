#include <cmath>
#include "raylib.h"

#define IMPLEMENTATION
#include "player.hpp"


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

    Player player;
    camera.target = player.Position;

    SetCameraMode(camera, CAMERA_THIRD_PERSON);
    SetTargetFPS(30);

    while (!WindowShouldClose()) {
        int forward = 0;
        if (IsKeyDown(KEY_W))
            forward = 1;
        if (IsKeyDown(KEY_S))
            forward = -1;
        if (IsKeyDown(KEY_A))
            player.Rotation = (player.Rotation + 10) % 360;
        if (IsKeyDown(KEY_D)) {
            player.Rotation -= 10;
            if (player.Rotation < 0)
                player.Rotation += 360;
        }

        player.Position.x += forward * 1.0f
            * std::cos((float) player.Rotation * PI / 180.0f);
        player.Position.z -= forward * 1.0f
            * std::sin((float) player.Rotation * PI / 180.0f);

        camera.target = player.Position;

        UpdateCamera(&camera);

        BeginDrawing();
        BeginMode3D(camera);
        ClearBackground(RAYWHITE);

        player.Draw();

        DrawGrid(100, 1.0f);
        EndMode3D();
        DrawFPS(10, 10);
        EndDrawing();

    }

    CloseWindow();

    return 0;
}
