#include <cmath>
#include <iostream>
#include <unistd.h>
#include <vector>

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define NET_IMPL
#define NBN_LogInfo(...)    {puts("[NET/INFO] "); printf(__VA_ARGS__); putchar('\n');}
#define NBN_LogDebug(...)   {puts("[NET/DEBUG] "); printf(__VA_ARGS__); putchar('\n');}
#define NBN_LogError(...)   {puts("[NET/ERROR] "); printf(__VA_ARGS__); putchar('\n');}
#define NBN_LogTrace(...)   {}
#define NBNET_IMPL
#include "net.hpp"

#define GRAPHIC_IMPL
#include "player.hpp"

#include "gui.hpp"

#define MS * 1000

std::vector<Player *> activePlayers;
Player *me = NULL;

Chatbox chatbox;


int handle_message()
{
    NBN_MessageInfo msg_info = NBN_GameClient_GetMessageInfo();
    switch ((NetType) msg_info.type) {
    case NetType::Arrive:
    {
        NetArrive *msg = (NetArrive *) msg_info.data;

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
        NetChat *msg = (NetChat *) msg_info.data;
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
        NetMove *msg = (NetMove *) msg_info.data;

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
        NetMove *msg = (NetMove *) msg_info.data;

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

#define REGISTER(T) { \
    NBN_GameClient_RegisterMessage((uint8_t) NetType::T,    \
        (NBN_MessageBuilder)    Net##T::New,                \
        (NBN_MessageDestructor) Net##T::Destroy,            \
        (NBN_MessageSerializer) Net##T::Serialize);         \
}


int main(int argc, char **argv)
{
    NBN_GameClient_Init(NET_PROTO, "127.0.0.1", NET_PORT);

    REGISTER(Arrive);
    REGISTER(Chat);
    REGISTER(Move);
    REGISTER(Leave);
    REGISTER(Login);

    if (NBN_GameClient_Start() < 0) {
        NBN_LogError("Start failed.");
        NBN_GameClient_Deinit();
    }

    SetTraceLogLevel(LOG_WARNING);

    InitWindow(1280, 720, "libreONLINE");

    char username[256];
    bool entered = false;

    while (!WindowShouldClose() && !me) {
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

        BeginDrawing();
        ClearBackground(WHITE);
        if (!entered) {
            if (GuiTextInputBox(Rectangle{300, 300, 400, 300},
                                "Login", "Enter a name:", "Login",
                                username) == 1) {
                NetLogin *pkt = NetLogin::New();
                strcpy(pkt->Name, username);
                NBN_OutgoingMessage *msg = NBN_GameClient_CreateMessage(
                        (uint8_t) NetType::Login, pkt);
                NBN_GameClient_SendReliableMessage(msg);

                entered = true;
            }
        } else {
            DrawText("Waiting for server...", 300, 300, 36, BLACK);
        }

        EndDrawing();
        NBN_GameClient_SendPackets();
    }

    if (!me) {
        // not yet logged in and window should be closed/errored, exit:
        CloseWindow();
        NBN_GameClient_Disconnect();
        NBN_GameClient_Stop();
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
            NetMove *pkt = NetMove::New();
            pkt->PosState = {
                .X = me->Position.x,
                .Y = me->Position.y,
                .Z = me->Position.z,
                .Rot = (unsigned int) me->Rotation,
            };
            NBN_OutgoingMessage *msg = NBN_GameClient_CreateMessage(
                    (uint8_t) NetType::Move, pkt);
            NBN_GameClient_SendReliableMessage(msg);
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

        NBN_GameClient_SendPackets();
    }

    CloseWindow();

    NBN_GameClient_Disconnect();
    NBN_GameClient_Stop();

    return 0;
}
