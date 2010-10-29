#import "Socket.h"

#undef self
#define self Client

class(self) {
	SocketConnection *conn;
	void *data;
};

ExtendClass(self);

def(void, Init);
def(void, Destroy);
def(ssize_t, GetFd);
def(void, Accept, Socket *socket);
