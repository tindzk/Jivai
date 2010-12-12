#import "Socket.h"

#define self SocketClient

class {
	SocketConnection *conn;
	void *data;
};

ExtendClass;

def(void, Init);
def(void, Destroy);
def(void, SetData, void *data);
def(void *, GetData);
def(SocketConnection *, GetConn);
def(ssize_t, GetFd);
def(void, Accept, Socket *socket);

#undef self
