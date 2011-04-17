#import "EventLoop.h"

#define self EventLoop

SingletonEx(self);
SingletonDestructor(self);

/* Note that this method does not close any file descriptors. Its only purpose
 * is to free the associated memory.
 */
static def(void, _DestroyEntry, GenericInstance inst) {
	ref(Entry) *entry = inst.object;

	if (entry->type == ref(EntryType_Client)) {
		ref(ClientEntry) *data = (void *) entry->data;
		delegate(data->client, onDestroy, data->data);
	}

	Pool_Free(Pool_GetInstance(), entry);
}

static def(void, OnEvent, int events, GenericInstance inst);

def(void, Init) {
	this->onTimeout = scall(OnTimeout_Empty);
	Poll_Init(&this->poll, Poll_OnEvent_For(this, ref(OnEvent)));
	DoublyLinkedList_Init(&this->entries);
}

def(void, Destroy) {
	DoublyLinkedList_Destroy(&this->entries,
		LinkedList_OnDestroy_For(this, ref(_DestroyEntry)));

	Poll_Destroy(&this->poll);
}

/* Listens for incoming connections. */
def(void, AttachSocket, Socket *socket, ref(OnConnection) onConnection) {
	ref(Entry) *entry = Pool_Alloc(Pool_GetInstance(),
		sizeof(ref(Entry)) + sizeof(ref(SocketEntry)));

	entry->type = ref(EntryType_Socket);

	ref(SocketEntry) *data = (void *) entry->data;

	data->cb     = onConnection;
	data->socket = socket;

	Poll_AddEvent(&this->poll, entry, socket->fd,
		Poll_Events_Error |
		Poll_Events_Input |
		Poll_Events_HangUp);

	DoublyLinkedList_InsertEnd(&this->entries, entry);
}

/* Accepts incoming connection and listens for data. */
def(ref(ClientEntry) *, AcceptClient, Socket *socket, bool edgeTriggered, ref(Client) client) {
	int flags = 0;

	if (edgeTriggered) {
		BitMask_Set(flags, Poll_Events_EdgeTriggered);
	}

	if (implements(client, onDestroy)) {
		BitMask_Set(flags, Poll_Events_Error);
		BitMask_Set(flags, Poll_Events_HangUp);
		BitMask_Set(flags, Poll_Events_PeerHangUp);
	}

	if (implements(client, onInput)) {
		BitMask_Set(flags, Poll_Events_Input);
	}

	if (implements(client, onOutput)) {
		BitMask_Set(flags, Poll_Events_Output);
	}

	assert(implements(client, getSize));

	ref(Entry) *entry = Pool_Alloc(Pool_GetInstance(),
		sizeof(ref(Entry))       +
		sizeof(ref(ClientEntry)) +
		delegate(client, getSize));

	entry->type = ref(EntryType_Client);

	ref(ClientEntry) *data = (void *) entry->data;
	data->client = client;

	data->conn = Socket_Accept(socket);
	data->conn.corking     = true;
	data->conn.nonblocking = true;

	Poll_AddEvent(&this->poll, entry, data->conn.fd, flags);

	DoublyLinkedList_InsertEnd(&this->entries, entry);

	return data;
}

def(void, _DetachClient, ref(Entry) *entry) {
	ref(ClientEntry) *data = (void *) entry->data;

	assert(entry->type == ref(EntryType_Client));

	DoublyLinkedList_Remove(&this->entries, entry);

	/* Closing the socket fd automatically unsubscribes from epoll. */
	SocketConnection_Close(&data->conn);

	call(_DestroyEntry, entry);
}

/* This also calls the set destruction methods. After calling this method,
 * associated memory will not be accessible any longer.
 */
def(void, DetachClient, GenericInstance inst) {
	ref(Entry) *entry = inst.object
		- sizeof(ref(ClientEntry))
		- sizeof(ref(Entry));

	call(_DetachClient, entry);
}

def(bool, IsRunning) {
	return this->running;
}

def(void, Iteration, int timeout) {
	Poll_Process(&this->poll, timeout);
	callback(this->onTimeout, timeout);
}

overload def(void, Run, int timeout) {
	this->running = true;
	while (this->running) {
		call(Iteration, timeout);
	}
}

def(void, Quit) {
	this->running = false;
}

static def(void, OnSocketEvent, int events, ref(SocketEntry) *entry) {
	if (BitMask_Has(events, Poll_Events_Input)) {
		callback(entry->cb, entry->socket);
	}
}

static def(void, OnClientEvent, int events, ref(Entry) *entry) {
	ref(ClientEntry) *data = (void *) entry->data;

	if (BitMask_Has(events,
			Poll_Events_Error  |
			Poll_Events_HangUp |
			Poll_Events_PeerHangUp))
	{
		/* Error occurred or connection hung up. */
		call(_DetachClient, entry);
		return;
	}

	if (BitMask_Has(events, Poll_Events_Input)) {
		/* Receiving data from client. */
		delegate(data->client, onInput, data->data);

		/* The connection could have been closed at this point (and thus entry's
		 * memory has become inaccessible). Therefore, we never emit `onInput'
		 * in conjunction with `onOutput'.
		 */
		return;
	}

	if (BitMask_Has(events, Poll_Events_Output)) {
		/* Client requests data. */
		delegate(data->client, onOutput, data->data);
	}
}

static def(void, OnEvent, int events, GenericInstance inst) {
	ref(Entry) *entry = inst.object;

	if (entry->type == ref(EntryType_Client)) {
		call(OnClientEvent, events, entry);
	} else if (entry->type == ref(EntryType_Socket)) {
		call(OnSocketEvent, events, (void *) entry->data);
	}
}
