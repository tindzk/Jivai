#import "GenericClientListener.h"

#define self GenericClientListener

def(void, Init, ConnectionInterface *conn, Logger *logger) {
	this->logger     = logger;
	this->connection = conn;
}

def(void, OnInit) {
	DoublyLinkedList_Init(&this->connections);
}

def(void, OnDestroy) {
	/* Shut down all remaining connections. */
	void (^destroy)(ClientConnection *) = ^(ClientConnection *conn) {
		this->connection->destroy(ClientConnection_GetData(conn));

		SocketClient_Destroy(conn->client);
		Pool_Free(Pool_GetInstance(), conn);
	};

	DoublyLinkedList_Destroy(&this->connections, destroy);
}

def(bool, OnConnect) {
	return true;
}

def(void, OnAccept, SocketClient *client) {
	ClientConnection *conn = ClientConnection_Alloc(this->connection->size);

	this->connection->init(
		ClientConnection_GetData(conn),
		SocketClient_GetConn(client),
		this->logger);

	/* Also save the client pointer because it's needed to force the
	 * closing of a connection. Otherwise the associated data could
	 * be destroyed, but not the actual connection.
	 */
	conn->client = SocketClient_GetObject(client);

	/* Link the connection with the current client. */
	SocketClient_SetData(client, conn);

	/* And add it to the list of currently active connections. */
	DoublyLinkedList_InsertEnd(&this->connections, conn);
}

def(void, OnDisconnect, SocketClient *client) {
	ClientConnection *conn = SocketClient_GetData(client);

	if (conn != NULL) {
		this->connection->destroy(
			ClientConnection_GetData(conn));

		DoublyLinkedList_Remove(&this->connections, conn);

		Pool_Free(Pool_GetInstance(), conn);
	}
}

static def(ClientConnection_Status, OnData, SocketClient *client, bool pull) {
	ClientConnection_Status status = ClientConnection_Status_Open;

	ClientConnection *conn = SocketClient_GetData(client);

	if (conn != NULL) {
		try {
			status = pull
				? this->connection->pull(ClientConnection_GetData(conn))
				: this->connection->push(ClientConnection_GetData(conn));
		} catch(SocketConnection, NotConnected) {
			status = ClientConnection_Status_Close;
		} catch(SocketConnection, ConnectionReset) {
			status = ClientConnection_Status_Close;
		} finally {

		} tryEnd;

		if (status == ClientConnection_Status_Close) {
			/* Destroy all data associated with the client. */
			call(OnDisconnect, client);

			/* Close the connection. */
			SocketClient_Destroy(client);
		}
	}

	return status;
}

def(ClientConnection_Status, OnPull, SocketClient *client) {
	return call(OnData, client, true);
}

def(ClientConnection_Status, OnPush, SocketClient *client) {
	return call(OnData, client, false);
}

Impl(ClientListener) = {
	.onInit             = ref(OnInit),
	.onDestroy          = ref(OnDestroy),
	.onClientConnect    = ref(OnConnect),
	.onClientAccept     = ref(OnAccept),
	.onPull             = ref(OnPull),
	.onPush             = ref(OnPush),
	.onClientDisconnect = ref(OnDisconnect)
};
