#import "Poll.h"
#import "Socket.h"
#import "Client.h"
#import "BitMask.h"
#import "Connection.h"
#import "ClientListenerInterface.h"

#ifndef Server_ConnectionLimit
#define Server_ConnectionLimit 1024
#endif

#define self Server

class {
	bool   edgeTriggered;
	Poll   poll;
	Socket socket;

	GenericInstance         context;
	ClientListenerInterface *listener;
};

def(void, Init, unsigned short port, ClientListenerInterface *listener, GenericInstance context);
def(void, Destroy);
def(void, SetEdgeTriggered, bool value);
def(void, Process);
def(void, DestroyClient, ClientInstance client);
def(void, AcceptClient);
def(void, OnEvent, int events, ClientInstance client);

#undef self
