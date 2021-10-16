#include <deque>
#include <string>

#include "raygui.h"

#include "player.hpp"
#include "net.hpp"


class Chatbox {
private:
    std::deque<std::string> logs;
    int debounce;

    int send()
    {
        NetChat *pkt = NetChat::New();
        strcpy(pkt->Message, Input);
        NBN_OutgoingMessage *msg = NBN_GameClient_CreateMessage(
                (uint8_t) NetType::Chat, pkt);
        NBN_GameClient_SendReliableMessage(msg);
        return 0;
    }
public:
    bool Active;
    char Input[256];

    Chatbox() : Input{}, Active{false}, debounce{0} {};
    void DoInput()
    {
        if (IsKeyDown(KEY_ENTER) && debounce == 0) {
            if (Active) {
                send(); // TODO: handle failure
                Input[0] = 0;
            }
            Active = !Active;
            debounce = 5;
        } else {
            if (debounce > 0)
                debounce--;
        }

        if (!Active)
            return;

        // shortcut to quickly clear all input
        if (IsKeyDown(KEY_BACKSPACE) && IsKeyDown(KEY_LEFT_SHIFT))
            Input[0] = 0;
    }

    void Draw2D()
    {
        int beginY = 650 - (logs.size() * 28);
        for (auto s:logs) {
            DrawText(s.c_str(), 10, beginY + 1, 26, BLUE);
            beginY += 28;
        }
        GuiSetStyle(DEFAULT, TEXT_SIZE, 26);
        GuiTextBox(Rectangle {10, 650, 450, 30}, Input, 256, Active);
    }

    void Receive(Player *from, const char *s)
    {
        logs.push_back(from->Name + std::string(": ") + std::string(s));
        if (logs.size() > 5)
            logs.pop_front();
    }
};
