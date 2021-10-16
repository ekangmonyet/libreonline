#ifndef __LO_INC_COMMON_HPP
#define __LO_INC_COMMON_HPP

#include "nbnet/nbnet.h"
#include "nbnet/net_drivers/udp.h"

#include <limits>

#define NET_PROTO   "LIBREONLINE0.1"
#define NET_PORT    8787


enum class NetType {
    ARRIVE = 0,
};

class NetArrive {
public:
    unsigned int PlayerId;
    bool IsYou;
    static NetArrive *New();
    static void Destroy(NetArrive *);
    static int Serialize(NetArrive *, NBN_Stream *);
};
#ifdef NET_IMPL
NetArrive *NetArrive::New()
{
    return new NetArrive{};
}

void NetArrive::Destroy(NetArrive *n)
{
    delete n;
}

int NetArrive::Serialize(NetArrive *n, NBN_Stream *s)
{
    NBN_SerializeUInt(s, n->PlayerId, 0, std::numeric_limits<unsigned int>().max());
    NBN_SerializeBool(s, n->IsYou);
    return 0;
}
#endif


#endif
