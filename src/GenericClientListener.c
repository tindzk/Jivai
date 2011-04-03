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
	void (^destroy)(ref(Connection) *) = ^(ref(Connection) *conn) {
		this->connection->destroy(&conn->object);

		SocketClient_Destroy(conn->client);
		Pool_Free(Pool_GetInstance(), conn);
	};

	DoublyLinkedList_Destroy(&this->connections, destroy);
}

def(bool, OnConnect) {
	return true;
}

def(void, OnAccept, SocketClient *client) {
	ref(Connection) *conn =
		Pool_Alloc(Pool_GetInstance(),
			sizeof(ref(Connection)) + this->connection->size);

	this->connection->init(&conn->object,
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
	ref(Connection) *conn = SocketClient_GetData(client);

	if (conn != NULL) {
		this->connection->destroy(&conn->object);

		DoublyLinkedList_Remove(&this->connections, conn);

		Pool_Free(Pool_GetInstance(), conn);
	}
}

static def(Connection_Status, OnData, SocketClient *client, bool pull) {
	Connection_Status status = Connection_Status_Open;

	ref(Connection) *conn = SocketClient_GetData(client);

	if (conn != NULL) {
		try {
			status = pull
				? this->connection->pull(&conn->object)
				: this->connection->push(&conn->object);
		} catch(SocketConnection, NotConnected) {
			status = Connection_Status_Close;
		} catch(SocketConnection, ConnectionReset) {
			status = Connection_Status_Close;
		} finally {

		} tryEnd;

		if (status == Connection_Status_Close) {
			/* Destroy all data associated with the client. */
			call(OnDisconnect, client);

			/* Close the connection. */
			SocketClient_Destroy(client);
		}
	}

	return status;
}

def(Connection_Status, OnPull, SocketClient *client) {
	return call(OnData, client, true);
}

def(Connection_Status, OnPush, SocketClient *client) {
	return call(OnData, client, false);
}

Impl(ClientListener) = {
	.onInit       = ref(OnInit),
	.onDestroy    = ref(OnDestroy),
	.onConnect    = ref(OnConnect),
	.onDisconnect = ref(OnDisconnect),
	.onAccept     = ref(OnAccept),
	.onPull       = ref(OnPull),
	.onPush       = ref(OnPush)
};
