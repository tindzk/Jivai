#include "Client.h"

Client* Client_New(void) {
	Client *client = New(Client);

	client->conn = NULL;
	client->data = NULL;

	return client;
}

void Client_Destroy(Client *client) {
	if (client->conn != NULL) {
		SocketConnection_Close(client->conn);
		Memory_Free(client->conn);
	}

	Memory_Free(client);
}

void Client_Accept(Client *client, Socket *socket) {
	SocketConnection conn = Socket_Accept(socket);

	client->conn = Memory_CloneObject(&conn);

	client->conn->corking     = true;
	client->conn->nonblocking = true;
}
