#import <errno.h>

#import "Fcntl.h"
#import "String.h"
#import "Exception.h"
#import "NetworkAddress.h"
#import "SocketConnection.h"

#undef self
#define self Socket

set(ref(Protocol)) {
	ref(Protocol_TCP),
	ref(Protocol_UDP)
};

enum {
	excAcceptFailed = excOffset,
	excAddressInUse,
	excBindFailed,
	excConnectFailed,
	excListenFailed,
	excSetSocketOption,
	excSocketFailed
};

class(self) {
	ssize_t fd;
	bool unused;
	Socket_Protocol protocol;
};

void Socket0(ExceptionManager *e);

def(void, Init, ref(Protocol) protocol);
def(void, SetNonBlockingFlag, bool enable);
def(void, SetCloexecFlag, bool enable);
def(void, SetReusableFlag, bool enable);
def(void, Listen, unsigned short port, int maxconns);
def(void, SetLinger);
def(SocketConnection, Connect, String hostname, unsigned short port);
def(SocketConnection, Accept);
def(void, Destroy);
