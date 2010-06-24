#include "ClientListener.h"

void ClientListener_Init(ClientListener *this, ConnectionInterface *itf, Server_Events *events) {
	this->connection           = itf;
	events->context            = this;
	events->onInit             = (void *) &ClientListener_OnInit;
	events->onDestroy          = (void *) &ClientListener_OnDestroy;
	events->onClientConnect    = (void *) &ClientListener_OnConnect;
	events->onClientAccept     = (void *) &ClientListener_OnAccept;
	events->onClientData       = (void *) &ClientListener_OnData;
	events->onClientDisconnect = (void *) &ClientListener_OnDisconnect;
}

void ClientListener_OnInit(ClientListener *this) {
	DoublyLinkedList_Init(&this->connections);
}

void ClientListener_OnDestroy(ClientListener *this) {
	/* Shut down all remaining connections. */
	void (^destroy)(Connection *) = ^(Connection *conn) {
		this->connection->destroy(conn);
		Client_Destroy(conn->client);
		Memory_Free(conn);
	};

	DoublyLinkedList_Destroy(&this->connections, destroy);
}

bool ClientListener_OnConnect(UNUSED ClientListener *this) {
	return true;
}

void ClientListener_OnAccept(ClientListener *this, Client *client) {
	Connection *conn = this->connection->new();

	this->connection->init(conn, client->conn);

	/* Also save the client pointer because it's needed to force the
	 * closing of a connection. Otherwise the associated data could
	 * be destroyed, but not the actual connection.
	 */
	conn->client = client;

	/* Link the connection with the current client. */
	client->data = conn;

	/* And add it to the list of currently active connections. */
	DoublyLinkedList_InsertEnd(&this->connections, conn);
}

void ClientListener_OnDisconnect(ClientListener *this, Client *client) {
	if (client->data != NULL) {
		Connection *conn = client->data;
		this->connection->destroy(conn);
		DoublyLinkedList_Remove(&this->connections, conn);
		Memory_Free(conn);
	}
}

void ClientListener_OnData(ClientListener *this, Client *client) {
	if (client->data != NULL) {
		Connection *conn = client->data;

		/* Keep the connection open? */
		bool close = this->connection->process(conn);

		if (close) {
			/* Destroy all data associated with the client. */
			ClientListener_OnDisconnect(this, client);

			/* Close the connection. */
			Client_Destroy(client);
		}
	}
}
