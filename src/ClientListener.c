#include "ClientListener.h"

static ExceptionManager *exc;

void ClientListener0(ExceptionManager *e) {
	exc = e;
}

void ClientListener_Init(ClientListener *this, ConnectionInterface *itf, Server_Events *events) {
	this->connection           = itf;
	events->context            = this;
	events->onInit             = (void *) &ClientListener_OnInit;
	events->onDestroy          = (void *) &ClientListener_OnDestroy;
	events->onClientConnect    = (void *) &ClientListener_OnConnect;
	events->onClientAccept     = (void *) &ClientListener_OnAccept;
	events->onPull             = (void *) &ClientListener_OnPull;
	events->onPush             = (void *) &ClientListener_OnPush;
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

static bool ClientListener_OnData(ClientListener *this, Client *client, bool pull) {
	bool close = false;

	if (client->data != NULL) {
		Connection *conn = client->data;

		try (exc) {
			close = pull
				? this->connection->pull(conn)
				: this->connection->push(conn);
		} catch(&SocketConnection_NotConnectedException, e) {
			close = true;
		} catch(&SocketConnection_ConnectionResetException, e) {
			close = true;
		} catch(&SocketConnection_LengthMismatchException, e) {
			close = true;
		} finally {

		} tryEnd;

		if (close) {
			/* Destroy all data associated with the client. */
			ClientListener_OnDisconnect(this, client);

			/* Close the connection. */
			Client_Destroy(client);
		}
	}

	return !close;
}

bool ClientListener_OnPull(ClientListener *this, Client *client) {
	if (this->connection->pull == NULL) {
		return true;
	}

	return ClientListener_OnData(this, client, true);
}

bool ClientListener_OnPush(ClientListener *this, Client *client) {
	if (this->connection->push == NULL) {
		return true;
	}

	return ClientListener_OnData(this, client, false);
}
