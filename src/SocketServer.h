#import "Socket.h"

#define self SocketServer

// @exc AddressInUse
// @exc AcceptFailed
// @exc BindFailed
// @exc ListenFailed
// @exc SetSocketOption

class {
	Socket socket;
};

rsdef(self, New, Socket_Protocol protocol, int options);
def(void, Destroy);
def(void, SetReusable, bool enable);
def(void, Bind, unsigned short port);
def(void, Listen, size_t maxconns);
def(SocketConnection, Accept, int options);

static alwaysInline def(Channel *, GetChannel) {
	return Socket_GetChannel(&this->socket);
}

#undef self
