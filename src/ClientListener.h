#ifndef CLIENT_LISTENER_H
#define CLIENT_LISTENER_H

#include "Server.h"
#include "Connection.h"
#include "ConnectionInterface.h"

typedef struct {
	Connections         connections;
	ConnectionInterface *connection;
} ClientListener;

void ClientListener0(ExceptionManager *e);
void ClientListener_Init(ClientListener *this, ConnectionInterface *itf, Server_Events *events);
void ClientListener_OnInit(ClientListener *this);
void ClientListener_OnDestroy(ClientListener *this);
bool ClientListener_OnConnect(UNUSED ClientListener *this);
void ClientListener_OnAccept(ClientListener *this, Client *client);
void ClientListener_OnDisconnect(ClientListener *this, Client *client);
static Connection_Status ClientListener_OnData(ClientListener *this, Client *client, bool pull);
Connection_Status ClientListener_OnPull(ClientListener *this, Client *client);
Connection_Status ClientListener_OnPush(ClientListener *this, Client *client);

#endif
