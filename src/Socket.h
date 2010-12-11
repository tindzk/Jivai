#import <errno.h>

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
// @exc FcntlFailed

class {
	ssize_t fd;
	bool unused;
	Socket_Protocol protocol;
};

def(void, Init, ref(Protocol) protocol);
def(void, SetNonBlockingFlag, bool enable);
def(void, SetCloexecFlag, bool enable);
def(void, SetReusableFlag, bool enable);
def(void, Listen, unsigned short port, int maxconns);
def(void, SetLinger);
def(SocketConnection, Connect, String hostname, unsigned short port);
def(SocketConnection, Accept);
def(void, Destroy);

#undef self
