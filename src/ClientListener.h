#import "Block.h"
#import "Server.h"
#import "Connection.h"
#import "ConnectionInterface.h"

#undef self
#define self ClientListener

class(self) {
	Connections         connections;
	ConnectionInterface *connection;
};

void ClientListener0(ExceptionManager *e);

def(void, Init, ConnectionInterface *itf, Server_Events *events);
def(void, OnInit);
def(void, OnDestroy);
def(bool, OnConnect);
def(void, OnAccept, Client *client);
def(void, OnDisconnect, Client *client);
def(Connection_Status, OnPull, Client *client);
def(Connection_Status, OnPush, Client *client);
