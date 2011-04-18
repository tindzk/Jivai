#import "Server.h"

#define self Server

rsdef(self, New, ConnectionInterface *conn, Logger *logger) {
	self res = (self) {
		.conn          = conn,
		.logger        = logger,
		.edgeTriggered = true,
		.socket        = Socket_New(Socket_Protocol_TCP)
	};

	Socket_SetCloexecFlag    (&res.socket, true);
	Socket_SetReusableFlag   (&res.socket, true);
	Socket_SetNonBlockingFlag(&res.socket, true);

	return res;
}

def(void, Destroy) {
	Socket_Destroy(&this->socket);

	/* The EventLoop may still have pointers to this instance. Therefore, we
	 * need to destroy it now before the object gets ivnalidated.
	 */
	EventLoop_Destroy(EventLoop_GetInstance());
}

def(void, SetEdgeTriggered, bool value) {
	this->edgeTriggered = value;
}

static def(void, OnData, ref(Client) *client, bool pull) {
	Connection_Status status = Connection_Status_Open;

	try {
		status = pull
			? this->conn->pull(client->object)
			: this->conn->push(client->object);
	} catch(SocketConnection, NotConnected) {
		status = Connection_Status_Close;
	} catch(SocketConnection, ConnectionReset) {
		status = Connection_Status_Close;
	} finally {

	} tryEnd;

	if (status == Connection_Status_Close) {
		EventLoop_DetachClient(EventLoop_GetInstance(), client);
	}
}

static def(void, OnDestroy, GenericInstance inst) {
	ref(Client) *client = inst.object;
	this->conn->destroy(client->object);
}

static def(void, OnPull, GenericInstance inst) {
	assert(this->conn->pull != NULL);
	call(OnData, inst.object, true);
}

static def(void, OnPush, GenericInstance inst) {
	assert(this->conn->push != NULL);
	call(OnData, inst.object, false);
}

static def(size_t, GetSize) {
	return sizeof(ref(Client)) + this->conn->size;
}

static def(void, OnConnection, Socket *socket) {
	EventLoop_ClientEntry *entry =
		EventLoop_AcceptClient(EventLoop_GetInstance(),
			socket, this->edgeTriggered, call(AsEventLoop_Client));

	ref(Client) *client = (void *) entry->data;

	client->client.conn  = &entry->conn;
	client->client.state = Connection_State_Established;

	this->conn->init(client->object, &client->client, this->logger);
}

def(void, Listen, unsigned short port) {
	Socket_Listen(&this->socket, port, ref(ConnectionLimit));

	EventLoop_AttachSocket(EventLoop_GetInstance(), &this->socket,
		EventLoop_OnConnection_For(this, ref(OnConnection)));
}

Impl(EventLoop_Client) = {
	.getSize   = ref(GetSize),
	.onInput   = ref(OnPull),
	.onOutput  = ref(OnPush),
	.onDestroy = ref(OnDestroy)
};
