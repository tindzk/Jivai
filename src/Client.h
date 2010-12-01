#import "Socket.h"

#define self Client

class {
	SocketConnection *conn;
	void *data;
};

ExtendClass;

def(void, Init);
def(void, Destroy);
def(ssize_t, GetFd);
def(void, Accept, Socket *socket);

#undef self
