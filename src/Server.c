#import "Server.h"

#define self Server

rsdef(self, New, ConnectionInterface *conn, Logger *logger) {
	self res = (self) {
		.conn          = conn,
		.logger        = logger,
		.edgeTriggered = true,
		.socket        = SocketServer_New(Socket_Protocol_TCP, Socket_Option_CloseOnExec)
	};

	SocketServer_SetReusable(&res.socket, true);

	return res;
}

def(void, Destroy) {
	SocketServer_Destroy(&this->socket);

	/* The EventLoop may still have pointers to this instance. Therefore, we
	 * need to destroy it now before the object gets ivnalidated.
	 */
	EventLoop_Destroy(EventLoop_GetInstance());
}

def(void, SetEdgeTriggered, bool value) {
	this->edgeTriggered = value;
}

static def(void, OnDestroy, ref(ClientDynInst) inst) {
	assert(this->conn->destroy != NULL);
	this->conn->destroy(inst.addr->object);
}

static def(void, OnPull, ref(ClientDynInst) inst) {
	assert(this->conn->pull != NULL);
	this->conn->pull(inst.addr->object);
}

static def(void, OnPush, ref(ClientDynInst) inst) {
	assert(this->conn->push != NULL);
	this->conn->push(inst.addr->object);
}

static def(size_t, GetSize) {
	return sizeof(ref(Client)) + this->conn->size;
}

static def(void, OnConnection, SocketServer *socket) {
	EventLoop_ClientEntry *entry =
		EventLoop_AcceptClient(EventLoop_GetInstance(),
			socket, this->edgeTriggered, call(AsEventLoop_Client));

	ref(Client) *client = (void *) entry->data;

	client->socket.conn  = &entry->conn;
	client->socket.state = Connection_State_Established;

	this->conn->init(client->object, client, this->logger);
}

def(void, Listen, unsigned short port) {
	SocketServer_Bind(&this->socket, port);
	SocketServer_Listen(&this->socket, ref(ConnectionLimit));

	EventLoop_AttachSocket(EventLoop_GetInstance(), &this->socket,
		EventLoop_OnConnection_For(this, ref(OnConnection)));
}

Impl(EventLoop_Client) = {
	.getSize   = ref(GetSize),
	.onInput   = ref(OnPull),
	.onOutput  = ref(OnPush),
	.onDestroy = ref(OnDestroy)
};
