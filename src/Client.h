#ifndef CLIENT_H
#define CLIENT_H

#include "Socket.h"

typedef struct {
	SocketConnection *conn;
	void *data;
} Client;

Client* Client_New(void);
void Client_Destroy(Client *client);
void Client_Accept(Client *client, Socket *socket);

#endif
