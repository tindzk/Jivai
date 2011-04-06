#import "Poll.h"
#import "Socket.h"
#import "BitMask.h"
#import "Connection.h"
#import "DoublyLinkedList.h"

#ifndef Server_ConnectionLimit
#define Server_ConnectionLimit 1024
#endif

#define self Server

record(ref(Client)) {
	DoublyLinkedList_DeclareRef(ref(Client));
	Connection_Client client;
	char              object[];
};

DoublyLinkedList_DeclareList(ref(Client), ref(Clients));

class {
	bool edgeTriggered;
	Poll poll;

	Socket socket;
	Logger *logger;

	ref(Clients)        clients;
	ConnectionInterface *conn;
};

def(void, Init, unsigned short port, ConnectionInterface *conn, Logger *logger);
def(void, Destroy);
def(void, SetEdgeTriggered, bool value);
def(void, Process);
def(void, DestroyClient, ref(Client) *client);

#undef self
