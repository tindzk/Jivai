#import "Socket.h"
#import "NetworkAddress.h"

#define self SocketClient

// @exc ConnectFailed

class {
	Socket_Protocol protocol;
	int options;
};

rsdef(self, New, Socket_Protocol protocol, int options);
def(void, Destroy);
def(SocketConnection, Connect, RdString hostname, unsigned short port);

#undef self
