#include <iostream>
#include <limits>

#define NBN_LogInfo(...)    {puts("[NET/INFO] "); printf(__VA_ARGS__); putchar('\n');}
#define NBN_LogDebug(...)   {puts("[NET/DEBUG] "); printf(__VA_ARGS__); putchar('\n');}
#define NBN_LogError(...)   {puts("[NET/ERROR] "); printf(__VA_ARGS__); putchar('\n');}
#define NBN_LogTrace(...)   {}
#define NBNET_IMPL
#include "nbnet/nbnet.h"
#include "nbnet/net_drivers/udp.h"

#define NET_IMPL
#include "net.hpp"

#define REGISTER(T) { \
    NBN_GameClient_RegisterMessage((uint8_t) NetType::T,    \
        (NBN_MessageBuilder)    Net##T::New,                \
        (NBN_MessageDestructor) Net##T::Destroy,            \
        (NBN_MessageSerializer) Net##T::Serialize);         \
}

int Net_Init()
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
        return 1;
    }

    return 0;
}

int Net_Poll(NetType *t, void **data)
{
    int ev = 0;

    if ((ev = NBN_GameClient_Poll()) == NBN_NO_EVENT)
        return Net_NO_EVENT;

    if (ev < 0) {
        NBN_LogError("Poll failed.");
        return ev;
    }
    switch (ev) {
    case NBN_CONNECTED:
        return 999;
    case NBN_DISCONNECTED:
        NBN_LogError("Disconnected.");
        return Net_DISCONNECTED;
    case NBN_MESSAGE_RECEIVED:
    {
        NBN_MessageInfo msg_info = NBN_GameClient_GetMessageInfo();
        *t = (NetType) msg_info.type;
        *data = msg_info.data;
        return Net_MESSAGE_RECEIVED;
    }
    }
    return -1;
}

void Net_Queue(NetType t, void *pkt)
{
    NBN_OutgoingMessage *msg = NBN_GameClient_CreateMessage((uint8_t) t, pkt);
    NBN_GameClient_SendReliableMessage(msg);
}

void Net_Send()
{
    NBN_GameClient_SendPackets();
}

void Net_Deinit()
{
    NBN_GameClient_Disconnect();
    NBN_GameClient_Stop();
}
