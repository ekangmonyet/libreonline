#include <iostream>
#include <unistd.h>
#include <vector>

#define NBN_LogInfo(...)    {puts("[NET/INFO] "); printf(__VA_ARGS__); putchar('\n');}
#define NBN_LogDebug(...)   {puts("[NET/DEBUG] "); printf(__VA_ARGS__); putchar('\n');}
#define NBN_LogError(...)   {puts("[NET/ERROR] "); printf(__VA_ARGS__); putchar('\n');}
#define NBN_LogTrace(...)   {}
#define IMPLEMENTATION
#define NBNET_IMPL
#include "net.hpp"

#define MS * 1000

static std::vector<NBN_Connection *> clients;

static int handle_message()
{
    NBN_MessageInfo msg_info = NBN_GameServer_GetMessageInfo();
    if (msg_info.type != (uint8_t) NetType::ARRIVE)
        return 1;
    std::cout << "ARRIVE: " << msg_info.sender->id <<std::endl;


    NetArrive *rpkt = NetArrive::New();

    for (auto c: clients) {
        if (c == msg_info.sender)
            continue;
        NBN_OutgoingMessage *relay = NBN_GameServer_CreateMessage(
                (uint8_t) NetType::ARRIVE, rpkt);
        // announce to other client
        NBN_GameServer_SendReliableMessageTo(c, relay);
        // announce other client
        NetArrive *pkt = NetArrive::New();
        NBN_OutgoingMessage *msg = NBN_GameServer_CreateMessage(
                (uint8_t) NetType::ARRIVE, pkt);
        NBN_GameServer_SendReliableMessageTo(msg_info.sender, msg);
    }
    return 0;
}

int main()
{
    NBN_GameServer_Init(NET_PROTO, NET_PORT);

    NBN_GameServer_RegisterMessage((uint8_t) NetType::ARRIVE,
            (NBN_MessageBuilder)    NetArrive::New,
            (NBN_MessageDestructor) NetArrive::Destroy,
            (NBN_MessageSerializer) NetArrive::Serialize);

    if (NBN_GameServer_Start() < 0) {
        NBN_LogError("Start failed.");
        NBN_GameServer_Deinit();
    }

    while (1) {
        int ev;
        while ((ev = NBN_GameServer_Poll()) != NBN_NO_EVENT) {
            if (ev < 0) {
                NBN_LogError("Poll failed.");
                break;
            }

            switch (ev) {
            case NBN_NEW_CONNECTION:
            {
                NBN_Connection *c = NBN_GameServer_GetIncomingConnection();
                std::cout << "Client connected: " << c->id << std::endl;
                clients.push_back(c);
                NBN_GameServer_AcceptIncomingConnection();
                break;
            }
            case NBN_CLIENT_DISCONNECTED:
            {
                int i = -1;
                for (auto c:clients) {
                    i++;
                    if (c->id != NBN_GameServer_GetDisconnectedClient()->id)
                        break;
                }
                if (i != -1) {
                    std::cout << "Client gone: " << clients[i]->id << std::endl;
                    clients.erase(clients.begin() + i);
                }
                break;
            }
            case NBN_CLIENT_MESSAGE_RECEIVED:
                handle_message();
                break;
            }
        }

        if (NBN_GameServer_SendPackets() < 0) {
            NBN_LogError("Send failed.");
            break;
        }

        usleep(100 MS);
    }

    NBN_GameServer_Stop();
    NBN_GameServer_Deinit();
}
