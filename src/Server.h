#ifndef SERVER_H
#define SERVER_H

#include "Poll.h"
#include "Socket.h"
#include "Client.h"
#include "BitMask.h"
#include "Connection.h"

#ifndef Server_ConnectionLimit
#define Server_ConnectionLimit SOMAXCONN
#endif

typedef void (* Server_OnInit)(void *);
typedef void (* Server_OnDestroy)(void *);
typedef bool (* Server_OnClientConnect)(void *);
typedef void (* Server_OnClientAccept)(void *, Client *);
typedef Connection_Status (* Server_OnClientData)(void *, Client *);
typedef void (* Server_OnClientDisconnect)(void *, Client *);

typedef struct {
	void *context;

	Server_OnInit             onInit;
	Server_OnInit             onDestroy;
	Server_OnClientConnect    onClientConnect;
	Server_OnClientAccept     onClientAccept;
	Server_OnClientData       onPull;
	Server_OnClientData       onPush;
	Server_OnClientDisconnect onClientDisconnect;
} Server_Events;

typedef struct {
	bool          edgeTriggered;
	Poll          poll;
	Socket        socket;
	Server_Events events;
} Server;

void Server0(ExceptionManager *e);

void Server_Init(Server *this, Server_Events events, bool edgeTriggered, int port);
void Server_Destroy(Server *this);
void Server_Process(Server *this);
void Server_DestroyClient(Server *this, Client *client);
void Server_AcceptClient(Server *this);
void Server_OnEvent(Server *this, int events, Client *client);

#endif
