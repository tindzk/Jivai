#import "GenericClientListener.h"

def(void, Init, ConnectionInterface *conn) {
	this->connection = conn;
}

def(void, OnInit) {
	DoublyLinkedList_Init(&this->connections);
}

def(void, OnDestroy) {
	/* Shut down all remaining connections. */
	void (^destroy)(Connection *) = ^(Connection *conn) {
		this->connection->destroy(conn);

		Client_Destroy(conn->client);
		Client_Free(conn->client);

		Memory_Free(conn);
	};

	DoublyLinkedList_Destroy(&this->connections, destroy);
}

def(bool, OnConnect) {
	return true;
}

def(void, OnAccept, Client *client) {
	Connection *conn = Memory_Alloc(this->connection->size);

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

def(void, OnDisconnect, Client *client) {
	if (client->data != NULL) {
		Connection *conn = client->data;
		this->connection->destroy(conn);
		DoublyLinkedList_Remove(&this->connections, conn);
		Memory_Free(conn);
	}
}

static def(Connection_Status, OnData, Client *client, bool pull) {
	Connection_Status status = Connection_Status_Open;

	if (client->data != NULL) {
		Connection *conn = client->data;

		try {
			status = pull
				? this->connection->pull(conn)
				: this->connection->push(conn);
		} clean catch(SocketConnection, excNotConnected) {
			status = Connection_Status_Close;
		} catch(SocketConnection, excConnectionReset) {
			status = Connection_Status_Close;
		} finally {

		} tryEnd;

		if (status == Connection_Status_Close) {
			/* Destroy all data associated with the client. */
			call(OnDisconnect, client);

			/* Close the connection. */
			Client_Destroy(client);

			Client_Free(client);
		}
	}

	return status;
}

def(Connection_Status, OnPull, Client *client) {
	return call(OnData, client, true);
}

def(Connection_Status, OnPush, Client *client) {
	return call(OnData, client, false);
}

Impl(ClientListener) = {
	.onInit             = (void *) ref(OnInit),
	.onDestroy          = (void *) ref(OnDestroy),
	.onClientConnect    = (void *) ref(OnConnect),
	.onClientAccept     = (void *) ref(OnAccept),
	.onPull             = (void *) ref(OnPull),
	.onPush             = (void *) ref(OnPush),
	.onClientDisconnect = (void *) ref(OnDisconnect)
};
