#ifndef __LO_INC_COMMON_HPP
#define __LO_INC_COMMON_HPP

#include "nbnet/nbnet.h"
#include "nbnet/net_drivers/udp.h"

#define NET_PROTO   "LIBREONLINE0.1"
#define NET_PORT    8787


enum class NetType {
    ARRIVE = 0,
};

class NetArrive {
public:
    static NetArrive *New();
    static void Destroy(NetArrive *);
    static int Serialize(NetArrive *, NBN_Stream *);
};
#ifdef IMPLEMENTATION
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
    return 0;
}
#endif


#endif
