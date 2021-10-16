#ifndef __LO_INC_COMMON_HPP
#define __LO_INC_COMMON_HPP

#include "nbnet/nbnet.h"
#include "nbnet/net_drivers/udp.h"

#include <limits>

#define NET_PROTO   "LIBREONLINE0.1"
#define NET_PORT    8787


class _PosState {
public:
    float X, Y, Z;
    unsigned int Rot;
    void Serialize(NBN_Stream *);
};

enum class NetType {
    Arrive = 0,
    Chat,
    Move,
    Leave,
    Login,
};

class NetArrive {
public:
    unsigned int PlayerId;
    bool IsYou;
    char Name[256];
    _PosState PosState;
    static NetArrive *New();
    static void Destroy(NetArrive *);
    static int Serialize(NetArrive *, NBN_Stream *);
};

class NetChat {
public:
    unsigned int PlayerId;
    char Message[256];
    static NetChat *New();
    static void Destroy(NetChat *);
    static int Serialize(NetChat *, NBN_Stream *);
};

class NetMove {
public:
    unsigned int PlayerId;
    _PosState PosState;
    static NetMove *New();
    static void Destroy(NetMove *);
    static int Serialize(NetMove *, NBN_Stream *);
};

class NetLeave {
public:
    unsigned int PlayerId;
    static NetLeave *New();
    static void Destroy(NetLeave *);
    static int Serialize(NetLeave *, NBN_Stream *);
};

class NetLogin {
public:
    char Name[256];
    static NetLogin *New();
    static void Destroy(NetLogin *);
    static int Serialize(NetLogin *, NBN_Stream *);
};

#ifdef NET_IMPL
const unsigned int UINTMAX = std::numeric_limits<unsigned int>().max();


NetArrive *NetArrive::New() { return new NetArrive{}; }
void NetArrive::Destroy(NetArrive *n) { delete n; }

int NetArrive::Serialize(NetArrive *n, NBN_Stream *s)
{
    NBN_SerializeUInt(s, n->PlayerId, 0, UINTMAX);
    NBN_SerializeBool(s, n->IsYou);
    NBN_SerializeString(s, n->Name, 256);
    n->PosState.Serialize(s);
    return 0;
}


NetChat *NetChat::New() { return new NetChat{}; }
void NetChat::Destroy(NetChat *n) { delete n; }

int NetChat::Serialize(NetChat *n, NBN_Stream *s)
{
    NBN_SerializeUInt(s, n->PlayerId, 0, UINTMAX);
    NBN_SerializeString(s, n->Message, 256);
    return 0;
}


NetMove *NetMove::New() { return new NetMove{}; }
void NetMove::Destroy(NetMove *n) { delete n; }

int NetMove::Serialize(NetMove *n, NBN_Stream *s)
{
    NBN_SerializeUInt(s, n->PlayerId, 0, UINTMAX);
    n->PosState.Serialize(s);
    return 0;
}


NetLeave *NetLeave::New() { return new NetLeave{}; }
void NetLeave::Destroy(NetLeave *n) { delete n; }

int NetLeave::Serialize(NetLeave *n, NBN_Stream *s)
{
    NBN_SerializeUInt(s, n->PlayerId, 0, UINTMAX);
    return 0;
}


NetLogin *NetLogin::New() { return new NetLogin{}; }
void NetLogin::Destroy(NetLogin *n) { delete n; }

int NetLogin::Serialize(NetLogin *n, NBN_Stream *s)
{
    NBN_SerializeString(s, n->Name, 256);
    return 0;
}


void _PosState::Serialize(NBN_Stream *s)
{
    NBN_SerializeFloat(s, X, -99999.0f, 99999.0f, 3);
    NBN_SerializeFloat(s, Y, -99999.0f, 99999.0f, 3);
    NBN_SerializeFloat(s, Z, -99999.0f, 99999.0f, 3);
    NBN_SerializeUInt(s, Rot, 0, 360);
}
#endif


#endif
