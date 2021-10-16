#ifndef __LO_INC_COMMON_HPP
#define __LO_INC_COMMON_HPP

#include "nbnet/nbnet.h"
#include "nbnet/net_drivers/udp.h"

#include <limits>

#define NET_PROTO   "LIBREONLINE0.1"
#define NET_PORT    8787


enum class NetType {
    Arrive = 0,
    Move,
};

class NetArrive {
public:
    unsigned int PlayerId;
    bool IsYou;
    static NetArrive *New();
    static void Destroy(NetArrive *);
    static int Serialize(NetArrive *, NBN_Stream *);
};

class NetMove {
public:
    unsigned int PlayerId;
    float X, Y, Z;
    unsigned int Rot;
    static NetMove *New();
    static void Destroy(NetMove *);
    static int Serialize(NetMove *, NBN_Stream *);
};

#ifdef NET_IMPL
const unsigned int UINTMAX = std::numeric_limits<unsigned int>().max();


NetArrive *NetArrive::New() { return new NetArrive{}; }
void NetArrive::Destroy(NetArrive *n) { delete n; }

int NetArrive::Serialize(NetArrive *n, NBN_Stream *s)
{
    NBN_SerializeUInt(s, n->PlayerId, 0, UINTMAX);
    NBN_SerializeBool(s, n->IsYou);
    return 0;
}


NetMove *NetMove::New() { return new NetMove{}; }
void NetMove::Destroy(NetMove *n) { delete n; }

int NetMove::Serialize(NetMove *n, NBN_Stream *s)
{
    NBN_SerializeUInt(s, n->PlayerId, 0, UINTMAX);
    NBN_SerializeFloat(s, n->X, -99999.0f, 99999.0f, 3);
    NBN_SerializeFloat(s, n->Y, -99999.0f, 99999.0f, 3);
    NBN_SerializeFloat(s, n->Z, -99999.0f, 99999.0f, 3);
    NBN_SerializeUInt(s, n->Rot, 0, 360);
    return 0;
}
#endif


#endif
