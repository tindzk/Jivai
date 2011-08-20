#import "BitMask.h"
#import "EventLoop.h"
#import "Connection.h"
#import "SocketServer.h"

#ifndef Server_ConnectionLimit
#define Server_ConnectionLimit 1024
#endif

#define self Server

record(ref(Socket)) {
	void            *inst;
	EventLoop_Entry *entry;
};

record(ref(Client)) {
	void              *inst;
	Connection_Client socket;
	EventLoop_Entry   *entry;
	char              object[];
};

Instance(ref(Client));

static inline void ref(Client_Push)(ref(Client) *client) {
	EventLoop_enqueue(EventLoop_GetInstance(), client->entry,
		ChannelWatcher_Events_Output);
}

static inline void ref(Client_Close)(ref(Client) *client) {
	EventLoop_enqueue(EventLoop_GetInstance(), client->entry,
		ChannelWatcher_Events_HangUp);
}

class {
	bool edgeTriggered;

	SocketServer socket;
	Logger *logger;

	ConnectionInterface *conn;
};

rsdef(self, new, ConnectionInterface *conn, Logger *logger);
def(void, destroy);
def(void, setEdgeTriggered, bool value);
def(void, listen, ushort port);

#undef self
