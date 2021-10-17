#include <cmath>
#include <iostream>
#include <unistd.h>
#include <vector>

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define GRAPHIC_IMPL
#include "player.hpp"

#include "net.hpp"
#include "gui.hpp"

#define MS * 1000

std::vector<Player *> activePlayers;
Player *me = NULL;

Chatbox chatbox;


int handle_message(NetType t, void *data)
{
    switch (t) {
    case NetType::Arrive:
    {
        NetArrive *msg = (NetArrive *) data;

        auto p = new Player();
        activePlayers.push_back(p);
        p->Id = msg->PlayerId;
        p->Name = std::string(msg->Name);
        _PosState *pos = &msg->PosState;
        p->Position = {pos->X, pos->Y, pos->Z};
        p->Rotation = pos->Rot;

        if (msg->IsYou) {
            me = activePlayers.back();
            p->Tint = RED;
        }
        break;
    }
    case NetType::Chat:
    {
        // TODO: we've used this one too many time, refactor
        // consider case in NetLeave too
        NetChat *msg = (NetChat *) data;
        Player *p = NULL;
        for (auto player:activePlayers)
            if (player->Id == msg->PlayerId) {
                p = player;
                break;
            }
        chatbox.Receive(p, msg->Message);
        break;
    }
    case NetType::Move:
    {
        NetMove *msg = (NetMove *) data;

        Player *p = NULL;
        for (auto player:activePlayers)
            if (player->Id == msg->PlayerId) {
                p = player;
                break;
            }
        // ASSUME p not NULL so we can catch immediately when it happens
        _PosState *pos = &msg->PosState;
        p->Position = {pos->X, pos->Y, pos->Z};
        p->Rotation = pos->Rot;
        break;
    }
    case NetType::Leave:
    {
        NetMove *msg = (NetMove *) data;

        int i = -1;
        for (auto player:activePlayers) {
            i++;
            if (player->Id == msg->PlayerId)
                break;
        }
        // Same as L55
        activePlayers.erase(activePlayers.begin() + i);
        break;
    }
    // Shouldnt receive these at all:
    case NetType::Login:
        break;
    }

    return 0;
}



int main(int argc, char **argv)
{
    Net_Init();

    SetTraceLogLevel(LOG_WARNING);

    InitWindow(1280, 720, "libreONLINE");

    char username[256];
    bool entered = false;

    while (!WindowShouldClose() && !me) {
        int ev, err = 0;
        NetType t; void *data;
        while ((ev = Net_Poll(&t, &data)) != Net_NO_EVENT) {
            if (ev < 0 || ev == Net_DISCONNECTED) {
                err = 1;
                break;
            }
            if (ev == Net_MESSAGE_RECEIVED)
                handle_message(t, data);
        }
        if (err != 0)
            break;

        BeginDrawing();
        ClearBackground(WHITE);
        if (!entered) {
            if (GuiTextInputBox(Rectangle{300, 300, 400, 300},
                                "Login", "Enter a name:", "Login",
                                username) == 1) {
                auto pkt = new NetLogin();
                strcpy(pkt->Name, username);
                Net_Queue(NetType::Login, pkt);

                entered = true;
            }
        } else {
            DrawText("Waiting for server...", 300, 300, 36, BLACK);
        }

        EndDrawing();
        Net_Send();
    }

    if (!me) {
        // not yet logged in and window should be closed/errored, exit:
        CloseWindow();
        Net_Deinit();
        return 1;
    }

    Camera camera {
        .position = {20.0f, 20.0f, 20.0f},
        .target = {0.0f, 0.0f, 0.0f},
        .up = {0.0f, 1.6f, 0.0f},
        .fovy = 45.0f,
        .projection = CAMERA_PERSPECTIVE,
    };

    camera.target = me->Position;

    SetCameraMode(camera, CAMERA_THIRD_PERSON);
    SetTargetFPS(30);

    int debounce = 0;

    while (!WindowShouldClose()) {
        int ev, err = 0;
        NetType t; void *data;
        while ((ev = Net_Poll(&t, &data)) != Net_NO_EVENT) {
            if (ev < 0 || ev == Net_DISCONNECTED) {
                err = 1;
                break;
            }
            if (ev == Net_MESSAGE_RECEIVED)
                handle_message(t, data);
        }
        if (err != 0)
            break;

        chatbox.DoInput();

        if (!chatbox.Active) {
            int forward = 0;
            if (IsKeyDown(KEY_W))
                forward = 1;
            if (IsKeyDown(KEY_S))
                forward = -1;
            if (IsKeyDown(KEY_A))
                me->Rotation = (me->Rotation + 10) % 360;
            if (IsKeyDown(KEY_D)) {
                me->Rotation -= 10;
                if (me->Rotation < 0)
                    me->Rotation += 360;
            }

            me->Position.x += forward * 1.0f
                * std::cos((float) me->Rotation * PI / 180.0f);
            me->Position.z -= forward * 1.0f
                * std::sin((float) me->Rotation * PI / 180.0f);

            // announce movement
            auto pkt = new NetMove();
            pkt->PosState = {
                .X = me->Position.x,
                .Y = me->Position.y,
                .Z = me->Position.z,
                .Rot = (unsigned int) me->Rotation,
            };
            Net_Queue(NetType::Move, pkt);
        }
        camera.target = me->Position;

        UpdateCamera(&camera);

        BeginDrawing();
        BeginMode3D(camera);
        ClearBackground(RAYWHITE);

        for (auto p:activePlayers)
            p->Draw();

        DrawGrid(100, 1.0f);
        EndMode3D();

        for (auto p:activePlayers)
            p->Draw2D(camera);

        DrawFPS(10, 10);

        chatbox.Draw2D();

        EndDrawing();

        Net_Send();
    }

    CloseWindow();

    Net_Deinit();

    return 0;
}
