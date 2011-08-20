#import "Server.h"

#define self Server

rsdef(self, new, ConnectionInterface *conn, Logger *logger) {
	self res = (self) {
		.conn          = conn,
		.logger        = logger,
		.edgeTriggered = true,
		.socket        = SocketServer_New(Socket_Protocol_TCP, Socket_Option_CloseOnExec)
	};

	SocketServer_SetReusable(&res.socket, true);

	return res;
}

def(void, destroy) {
	SocketServer_Destroy(&this->socket);

	/* The EventLoop may still have pointers to this instance.
	 * Therefore, we need to destroy it now before the object gets
	 * invalidated.
	 * Only delete those entries that were registered by this class.
	 */
	EventLoop_pullDown(EventLoop_GetInstance(), this);
}

def(void, setEdgeTriggered, bool value) {
	this->edgeTriggered = value;
}

static sdef(void, onDestroyConnection, Instance inst) {
	ref(Client) *client = inst.addr - sizeof(ref(Client));

	InstName(self) $this = { .addr = client->inst };
	if (this->conn->destroy != null) {
		this->conn->destroy(client->object);
	}

	/* Closing the socket channel automatically unsubscribes from epoll. */
	SocketConnection_Destroy(&client->socket.conn);
	EventLoop_detach(EventLoop_GetInstance(), client->entry, false);
}

/* Accepts an incoming connection and listens for data. */
static sdef(void, onConnection, Instance inst) {
	ref(Socket) *socket = inst.addr;

	InstName(self) $this = { .addr = socket->inst };

	EventLoop_Entry *entry = EventLoop_createEntry(
		EventLoop_GetInstance(), this,
		sizeof(ref(Client)) + this->conn->size);

	ref(Client) *client = (void *) entry->data;

	client->inst         = this;
	client->socket.conn  = SocketServer_Accept(&this->socket, Socket_Option_CloseOnExec);
	client->socket.state = Connection_State_Established;
	client->entry        = entry;

	SocketConnection_SetCorking(&client->socket.conn, true);

	EventLoop_Options opts = {
		.ch            = SocketConnection_GetChannel(&client->socket.conn),
		.edgeTriggered = this->edgeTriggered,
		.events        = {
			.inst      = { .addr = client->object },
			.onDestroy = ref(onDestroyConnection),
			.onInput   = this->conn->pull,
			.onOutput  = this->conn->push
		}
	};

	EventLoop_attach(EventLoop_GetInstance(), entry, opts);

	this->conn->init(client->object, client, this->logger);
}

static sdef(void, onDestroySocket, Instance inst) {
	ref(Socket) *socket = inst.addr;

	EventLoop_detach(EventLoop_GetInstance(), socket->entry, false);
}

/* Listens for incoming connections. */
def(void, listen, ushort port) {
	SocketServer_Bind(&this->socket, port);
	SocketServer_Listen(&this->socket, ref(ConnectionLimit));

	EventLoop_Entry *entry = EventLoop_createEntry(
		EventLoop_GetInstance(), this, sizeof(ref(Socket)));

	ref(Socket) *socket = (void *) entry->data;

	socket->inst  = this;
	socket->entry = entry;

	EventLoop_Options opts = {
		.ch     = SocketServer_GetChannel(&this->socket),
		.events = {
			.inst      = { .addr = socket },
			.onDestroy = ref(onDestroySocket),
			.onInput   = ref(onConnection)
		}
	};

	EventLoop_attach(EventLoop_GetInstance(), entry, opts);
}
