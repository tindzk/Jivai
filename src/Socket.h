#import "String.h"
#import "Kernel.h"
#import "Exception.h"
#import "NetworkAddress.h"
#import "SocketConnection.h"

#define self Socket

set(ref(Protocol)) {
	ref(Protocol_TCP),
	ref(Protocol_UDP)
};

// @exc AcceptFailed
// @exc AddressInUse
// @exc BindFailed
// @exc ConnectFailed
// @exc ListenFailed
// @exc SetSocketOption
// @exc SocketFailed

class {
	Channel ch;
	bool unused;
	Socket_Protocol protocol;
};

rsdef(self, New, ref(Protocol) protocol);
def(void, Destroy);
def(void, SetReusable, bool enable);
def(void, SetLinger);
def(void, Listen, unsigned short port, int maxconns);
def(SocketConnection, Connect, RdString hostname, unsigned short port);
def(SocketConnection, Accept);

#undef self
