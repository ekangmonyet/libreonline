#ifndef __LO_INC_COMMON_HPP
#define __LO_INC_COMMON_HPP

#define NET_PROTO   "LIBREONLINE0.1"
#define NET_PORT    8787


class _PosState {
public:
    float X, Y, Z;
    unsigned int Rot;
#ifdef NET_IMPL
    void Serialize(NBN_Stream *);
#endif
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
#ifdef NET_IMPL
    static NetArrive *New();
    static void Destroy(NetArrive *);
    static int Serialize(NetArrive *, NBN_Stream *);
#endif
};

class NetChat {
public:
    unsigned int PlayerId;
    char Message[256];
#ifdef NET_IMPL
    static NetChat *New();
    static void Destroy(NetChat *);
    static int Serialize(NetChat *, NBN_Stream *);
#endif
};

class NetMove {
public:
    unsigned int PlayerId;
    _PosState PosState;
#ifdef NET_IMPL
    static NetMove *New();
    static void Destroy(NetMove *);
    static int Serialize(NetMove *, NBN_Stream *);
#endif
};

class NetLeave {
public:
    unsigned int PlayerId;
#ifdef NET_IMPL
    static NetLeave *New();
    static void Destroy(NetLeave *);
    static int Serialize(NetLeave *, NBN_Stream *);
#endif
};

class NetLogin {
public:
    char Name[256];
#ifdef NET_IMPL
    static NetLogin *New();
    static void Destroy(NetLogin *);
    static int Serialize(NetLogin *, NBN_Stream *);
#endif
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

enum {
    Net_NO_EVENT = 0,
    Net_DISCONNECTED,
    Net_MESSAGE_RECEIVED,
};

int Net_Init();
int Net_Poll(NetType *t, void **data);
void Net_Queue(NetType t, void *pkt);
void Net_Send();
void Net_Deinit();
#endif
