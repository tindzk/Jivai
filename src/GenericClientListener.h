#import "Block.h"
#import "Logger.h"
#import "Server.h"
#import "Connection.h"
#import "ClientConnection.h"
#import "ClientListenerInterface.h"

#define self GenericClientListener

class {
	Logger              *logger;
	ClientConnections   connections;
	ConnectionInterface *connection;
};

def(void, Init, ConnectionInterface *conn, Logger *logger);
def(void, OnInit);
def(void, OnDestroy);
def(bool, OnConnect);
def(void, OnAccept, SocketClient *client);
def(void, OnDisconnect, SocketClient *client);
def(ClientConnection_Status, OnPull, SocketClient *client);
def(ClientConnection_Status, OnPush, SocketClient *client);

ExportImpl(ClientListener);

#undef self
