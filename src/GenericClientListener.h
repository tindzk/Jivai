#import "Block.h"
#import "Server.h"
#import "Connection.h"
#import "ConnectionInterface.h"
#import "ClientListenerInterface.h"

#define self GenericClientListener

class {
	Connections         connections;
	ConnectionInterface *connection;
};

def(void, Init, ConnectionInterface *conn);
def(void, OnInit);
def(void, OnDestroy);
def(bool, OnConnect);
def(void, OnAccept, Client *client);
def(void, OnDisconnect, Client *client);
def(Connection_Status, OnPull, Client *client);
def(Connection_Status, OnPush, Client *client);

extern Impl(ClientListener);

#undef self
