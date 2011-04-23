#import "Poll.h"
#import "Socket.h"
#import "BitMask.h"
#import "EventLoop.h"
#import "Connection.h"

#ifndef Server_ConnectionLimit
#define Server_ConnectionLimit 1024
#endif

#define self Server

record(ref(Client)) {
	Connection_Client socket;
	char              object[];
};

static inline void ref(Client_Close)(ref(Client) *client) {
	EventLoop_DetachClient(EventLoop_GetInstance(), client);
}

class {
	bool edgeTriggered;

	Socket socket;
	Logger *logger;

	ConnectionInterface *conn;
};

rsdef(self, New, ConnectionInterface *conn, Logger *logger);
def(void, Destroy);
def(void, SetEdgeTriggered, bool value);
def(void, Listen, unsigned short port);

ExportImpl(EventLoop_Client);

#undef self
