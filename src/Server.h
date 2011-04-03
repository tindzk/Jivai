#import "Poll.h"
#import "Socket.h"
#import "BitMask.h"
#import "SocketClient.h"
#import "ClientListener.h"

#ifndef Server_ConnectionLimit
#define Server_ConnectionLimit 1024
#endif

#define self Server

class {
	bool edgeTriggered;
	Poll poll;

	Socket socket;

	ClientListener listener;
};

def(void, Init, unsigned short port, ClientListener listener);
def(void, Destroy);
def(void, SetEdgeTriggered, bool value);
def(void, Process);
def(void, DestroyClient, SocketClient *client);
def(void, AcceptClient);
def(void, OnEvent, int events, SocketClientExtendedInstance instClient);

#undef self
