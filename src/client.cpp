#include <cmath>
#include <iostream>
#include <unistd.h>

#include "raylib.h"

#define IMPLEMENTATION
#include "player.hpp"

#define NBN_LogInfo(...)    {puts("[NET/INFO] "); printf(__VA_ARGS__); putchar('\n');}
#define NBN_LogDebug(...)   {puts("[NET/DEBUG] "); printf(__VA_ARGS__); putchar('\n');}
#define NBN_LogError(...)   {puts("[NET/ERROR] "); printf(__VA_ARGS__); putchar('\n');}
#define NBN_LogTrace(...)   {}
#define NBNET_IMPL
#include "net.hpp"

#define MS * 1000

int handle_message()
{
    NBN_MessageInfo msg_info = NBN_GameClient_GetMessageInfo();
    if (msg_info.type != (uint8_t) NetType::ARRIVE)
        return 1;
    std::cout << "ARRIVE" <<std::endl;
    return 0;
}

int main()
{
    NBN_GameClient_Init(NET_PROTO, "127.0.0.1", NET_PORT);
    NBN_GameClient_RegisterMessage((uint8_t) NetType::ARRIVE,
            (NBN_MessageBuilder)    NetArrive::New,
            (NBN_MessageDestructor) NetArrive::Destroy,
            (NBN_MessageSerializer) NetArrive::Serialize);

    if (NBN_GameClient_Start() < 0) {
        NBN_LogError("Start failed.");
        NBN_GameClient_Deinit();
    }

    // TODO: Check for connection!

    NetArrive *pkt = NetArrive::New();
    NBN_OutgoingMessage *msg = NBN_GameClient_CreateMessage(
            (uint8_t) NetType::ARRIVE, pkt);
    NBN_GameClient_SendReliableMessage(msg);
    NBN_GameClient_SendPackets();

    std::cout << "Finished" << std::endl;

    SetTraceLogLevel(LOG_WARNING);

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
        int ev, err = 0;
        while ((ev = NBN_GameClient_Poll()) != NBN_NO_EVENT) {
            if (ev < 0) {
                NBN_LogError("Poll failed.");
                err = 1;
                break;
            }
            switch (ev) {
            case NBN_DISCONNECTED:
                NBN_LogError("Disconnected.");
                err = 1;
                break;
            case NBN_MESSAGE_RECEIVED:
                handle_message();
                break;
            }
            if (err != 0)
                break;
        }

        if (err != 0)
            break;

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

    NBN_GameClient_Disconnect();
    NBN_GameClient_Stop();

    return 0;
}
