#import "Block.h"
#import "Server.h"
#import "ClientConnection.h"
#import "ConnectionInterface.h"
#import "ClientListenerInterface.h"

#define self GenericClientListener

class {
	ClientConnections    connections;
	ConnectionInterface *connection;
};

def(void, Init, ConnectionInterface *conn);
def(void, OnInit);
def(void, OnDestroy);
def(bool, OnConnect);
def(void, OnAccept, Client *client);
def(void, OnDisconnect, Client *client);
def(ClientConnection_Status, OnPull, Client *client);
def(ClientConnection_Status, OnPush, Client *client);

ExportImpl(ClientListener);

#undef self
