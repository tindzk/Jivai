#import "Poll.h"
#import "Socket.h"
#import "Client.h"
#import "BitMask.h"
#import "Connection.h"

#ifndef Server_ConnectionLimit
#define Server_ConnectionLimit 1024
#endif

#undef self
#define self Server

typedef void (* ref(OnInit))(void *);
typedef void (* ref(OnDestroy))(void *);
typedef bool (* ref(OnClientConnect))(void *);
typedef void (* ref(OnClientAccept))(void *, Client *);
typedef Connection_Status (* ref(OnClientData))(void *, Client *);
typedef void (* ref(OnClientDisconnect))(void *, Client *);

record(ref(Events)) {
	void *context;

	ref(OnInit)             onInit;
	ref(OnInit)             onDestroy;
	ref(OnClientConnect)    onClientConnect;
	ref(OnClientAccept)     onClientAccept;
	ref(OnClientData)       onPull;
	ref(OnClientData)       onPush;
	ref(OnClientDisconnect) onClientDisconnect;
};

class(self) {
	bool        edgeTriggered;
	Poll        poll;
	Socket      socket;
	ref(Events) events;
};

void Server0(ExceptionManager *e);

def(void, Init, ref(Events) events, bool edgeTriggered, unsigned short port);
def(void, Destroy);
def(void, Process);
def(void, DestroyClient, Client *client);
def(void, AcceptClient);
def(void, OnEvent, int events, Client *client);
