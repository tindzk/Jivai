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
def(void, OnAccept, SocketClientInstance client);
def(void, OnDisconnect, SocketClientInstance client);
def(ClientConnection_Status, OnPull, SocketClientInstance client);
def(ClientConnection_Status, OnPush, SocketClientInstance client);

ExportImpl(ClientListener);

#undef self
