#include <iostream>
#include <unistd.h>
#include <vector>

#define NET_IMPL
#define NBN_LogInfo(...)    {puts("[NET/INFO] "); printf(__VA_ARGS__); putchar('\n');}
#define NBN_LogDebug(...)   {puts("[NET/DEBUG] "); printf(__VA_ARGS__); putchar('\n');}
#define NBN_LogError(...)   {puts("[NET/ERROR] "); printf(__VA_ARGS__); putchar('\n');}
#define NBN_LogTrace(...)   {}
#define NBNET_IMPL
#include "net.hpp"

#define BASIC_IMPL
#include "player.hpp"

#define MS * 1000

class Client {
public:
    NBN_Connection *C;
    unsigned int PlayerId;
};

static std::vector<Player> players;
static std::vector<Client> clients;

static unsigned int ctr = 0;

static void handle_new()
{
    NBN_Connection *c = NBN_GameServer_GetIncomingConnection();
    NBN_GameServer_AcceptIncomingConnection();

    std::cout << "Client connected: " << c->id << std::endl;
    auto p = Player();
    p.Id = ctr++;

    NBN_OutgoingMessage *om;
    {
        NetArrive *pkt = NetArrive::New();
        pkt->IsYou = false;
        pkt->PlayerId = p.Id;
        om = NBN_GameServer_CreateMessage(
                (uint8_t) NetType::Arrive, pkt);
    }

    {
        // to client
        NetArrive *pkt = NetArrive::New();
        pkt->IsYou = true;
        pkt->PlayerId = p.Id;
        NBN_OutgoingMessage *cm = NBN_GameServer_CreateMessage(
                (uint8_t) NetType::Arrive, pkt);
        NBN_GameServer_SendReliableMessageTo(c, cm);
    }
    // sync client list
    for (auto other:clients) {
        NetArrive *pkt = NetArrive::New();
        pkt->IsYou = false;
        pkt->PlayerId = other.PlayerId;

        // to client
        NBN_OutgoingMessage *m = NBN_GameServer_CreateMessage(
                (uint8_t) NetType::Arrive, pkt);
        NBN_GameServer_SendReliableMessageTo(c, m);

        // to other
        NBN_GameServer_SendReliableMessageTo(other.C, om);
    }

    players.push_back(p);
    clients.push_back(Client{c, p.Id});
}

static int handle_message()
{
    NBN_MessageInfo msg_info = NBN_GameServer_GetMessageInfo();
    switch((NetType) msg_info.type) {
    case NetType::Arrive:
        break;
    case NetType::Move:
    {
        int playerId = -1;
        for (auto c: clients)
            if (c.C == msg_info.sender) {
                playerId = c.PlayerId;
                break;
            }
        NetMove *msg = (NetMove *) msg_info.data;
        // update player state
        _PosState *posState = &msg->PosState;
        players[playerId].Position = {posState->X, posState->Y, posState->Z};
        players[playerId].Rotation = posState->Rot;

        // sanitize message before relay
        msg->PlayerId = playerId;

        // announce to other player
        NBN_OutgoingMessage *m = NBN_GameServer_CreateMessage(
                (uint8_t) NetType::Move, msg);
        for (auto c: clients) {
            if (c.PlayerId == playerId)
                continue;
            NBN_GameServer_SendReliableMessageTo(c.C, m);
        }
        break;
    }
    }
    return 0;
}

#define REGISTER(T) { \
    NBN_GameServer_RegisterMessage((uint8_t) NetType::T,    \
        (NBN_MessageBuilder)    Net##T::New,                \
        (NBN_MessageDestructor) Net##T::Destroy,            \
        (NBN_MessageSerializer) Net##T::Serialize);         \
}


int main()
{
    NBN_GameServer_Init(NET_PROTO, NET_PORT);

    REGISTER(Arrive);
    REGISTER(Move);

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
                handle_new();
                break;
            case NBN_CLIENT_DISCONNECTED:
            {
                int i = -1;
                for (auto c:clients) {
                    i++;
                    if (c.C->id != NBN_GameServer_GetDisconnectedClient()->id)
                        break;
                }
                if (i != -1) {
                    std::cout << "Client gone: " << clients[i].C->id << std::endl;
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
