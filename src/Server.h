#import "Poll.h"
#import "Block.h"
#import "Socket.h"
#import "BitMask.h"
#import "Connection.h"
#import "SocketClient.h"

#ifndef Server_ConnectionLimit
#define Server_ConnectionLimit 1024
#endif

#define self Server

class {
	bool edgeTriggered;
	Poll poll;

	Socket socket;
	Logger *logger;

	SocketClients       conns;
	ConnectionInterface *conn;
};

def(void, Init, unsigned short port, ConnectionInterface *conn, Logger *logger);
def(void, Destroy);
def(void, SetEdgeTriggered, bool value);
def(void, Process);
def(void, DestroyClient, SocketClient *client);
def(void, OnEvent, int events, SocketClientExtendedInstance instClient);

#undef self
