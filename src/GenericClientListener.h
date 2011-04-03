#import "Block.h"
#import "Logger.h"
#import "Server.h"
#import "Connection.h"
#import "ClientListener.h"
#import "ClientConnection.h"

#define self GenericClientListener

record(ref(Connection)) {
	DoublyLinkedList_DeclareRef(ref(Connection));
	SocketClient *client;
	char object[];
};

DoublyLinkedList_DeclareList(ref(Connection), ref(Connections));

class {
	Logger              *logger;
	ref(Connections)    connections;
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
