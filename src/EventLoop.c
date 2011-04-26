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

def(void, Init) {
	this->onTimeout = scall(OnTimeout_Empty);
	this->watcher   = ChannelWatcher_New();
	this->queue     = EventQueue_New();
	DoublyLinkedList_Init(&this->entries);
}

def(void, Destroy) {
	DoublyLinkedList_Destroy(&this->entries,
		LinkedList_OnDestroy_For(this, ref(_DestroyEntry)));

	EventQueue_Destroy(&this->queue);

	ChannelWatcher_Destroy(&this->watcher);
}

def(ref(Entry) *, AddChannel, Channel *ch, ref(OnInput) onInput) {
	ref(Entry) *entry = Pool_Alloc(Pool_GetInstance(),
		sizeof(ref(Entry)) + sizeof(ref(ChannelEntry)));

	entry->type = ref(EntryType_Channel);

	ref(ChannelEntry) *data = (void *) entry->data;

	data->ch = ch;
	data->cb = onInput;

	ChannelWatcher_Subscribe(&this->watcher, data->ch, ChannelWatcher_Events_Input, entry);

	DoublyLinkedList_InsertEnd(&this->entries, entry);

	return entry;
}

/* Set `watcher' to true if the fd continues to be valid. Otherwise the kernel
 * removes the watcher automatically upon closure.
 */
def(void, DetachChannel, ref(Entry) *entry, bool watcher) {
	ref(ChannelEntry) *data = (void *) entry->data;

	assert(entry->type == ref(EntryType_Channel));

	if (watcher) {
		ChannelWatcher_Unsubscribe(&this->watcher, data->ch);
	}

	DoublyLinkedList_Remove(&this->entries, entry);

	call(_DestroyEntry, entry);
}

/* Listens for incoming connections. */
def(void, AttachSocket, SocketServer *socket, ref(OnConnection) onConnection) {
	ref(Entry) *entry = Pool_Alloc(Pool_GetInstance(),
		sizeof(ref(Entry)) + sizeof(ref(SocketEntry)));

	entry->type = ref(EntryType_Socket);

	ref(SocketEntry) *data = (void *) entry->data;

	data->cb     = onConnection;
	data->socket = socket;

	ChannelWatcher_Subscribe(&this->watcher, SocketServer_GetChannel(socket),
		ChannelWatcher_Events_Error |
		ChannelWatcher_Events_Input |
		ChannelWatcher_Events_HangUp,
		entry);

	DoublyLinkedList_InsertEnd(&this->entries, entry);
}

/* Accepts incoming connection and listens for data. */
def(ref(ClientEntry) *, AcceptClient, SocketServer *socket, bool edgeTriggered, ref(Client) client) {
	int flags = 0;

	if (edgeTriggered) {
		BitMask_Set(flags, ChannelWatcher_Events_EdgeTriggered);
	}

	if (implements(client, onDestroy)) {
		BitMask_Set(flags, ChannelWatcher_Events_Error);
		BitMask_Set(flags, ChannelWatcher_Events_HangUp);
		BitMask_Set(flags, ChannelWatcher_Events_PeerHangUp);
	}

	if (implements(client, onInput)) {
		BitMask_Set(flags, ChannelWatcher_Events_Input);
	}

	if (implements(client, onOutput)) {
		BitMask_Set(flags, ChannelWatcher_Events_Output);
	}

	assert(implements(client, getSize));

	ref(Entry) *entry = Pool_Alloc(Pool_GetInstance(),
		sizeof(ref(Entry))       +
		sizeof(ref(ClientEntry)) +
		delegate(client, getSize));

	entry->type = ref(EntryType_Client);

	ref(ClientEntry) *data = (void *) entry->data;
	data->client = client;

	data->conn = SocketServer_Accept(socket, Socket_Option_CloseOnExec);

	SocketConnection_SetCorking(&data->conn, true);

	ChannelWatcher_Subscribe(&this->watcher,
		SocketConnection_GetChannel(&data->conn), flags, entry);

	DoublyLinkedList_InsertEnd(&this->entries, entry);

	return data;
}

def(void, _DetachClient, ref(Entry) *entry) {
	ref(ClientEntry) *data = (void *) entry->data;

	assert(entry->type == ref(EntryType_Client));

	DoublyLinkedList_Remove(&this->entries, entry);

	/* Closing the socket fd automatically unsubscribes from ewatcher. */
	SocketConnection_Destroy(&data->conn);

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

static def(void, OnChannelEvent, int events, ref(Entry) *entry) {
	ref(ChannelEntry) *data = (void *) entry->data;

	if (BitMask_Has(events, ChannelWatcher_Events_Input)) {
		callback(data->cb);
	}
}

static def(void, OnSocketEvent, int events, ref(SocketEntry) *entry) {
	if (BitMask_Has(events, ChannelWatcher_Events_Input)) {
		callback(entry->cb, entry->socket);
	}
}

static def(void, OnClientEvent, int events, ref(Entry) *entry) {
	ref(ClientEntry) *data = (void *) entry->data;

	if (BitMask_Has(events,
			ChannelWatcher_Events_Error  |
			ChannelWatcher_Events_HangUp |
			ChannelWatcher_Events_PeerHangUp))
	{
		/* Error occurred or connection hung up. */
		call(_DetachClient, entry);
		return;
	}

	if (BitMask_Has(events, ChannelWatcher_Events_Input)) {
		/* Receiving data from client. */
		delegate(data->client, onInput, data->data);
	}

	if (BitMask_Has(events, ChannelWatcher_Events_Output)) {
		/* Client requests data. */
		delegate(data->client, onOutput, data->data);
	}
}

static def(void, OnEvent, int events, GenericInstance inst) {
	EventQueue_Enqueue(&this->queue, inst.object, events);
}

def(void, Iteration, int timeout) {
	/* Poll for new events and insert into event queue. */
	ChannelWatcher_Poll(&this->watcher,
		ChannelWatcher_OnEvent_For(this, ref(OnEvent)), timeout);

	while (EventQueue_HasEvents(&this->queue)) {
		/* Pop an event and process it. Note that each event can enqueue further
		 * events. This was also the reason for using a queue in the first
		 * place.
		 * To prevent that complex code paths emerge, a queue enables us to
		 * maintain a central point from which all events emanate from.
		 */
		EventQueue_Event event = EventQueue_Pop(&this->queue);

		ref(Entry) *entry = event.ptr;

		if (entry->type == ref(EntryType_Channel)) {
			call(OnChannelEvent, event.flags, entry);
		} else if (entry->type == ref(EntryType_Client)) {
			call(OnClientEvent, event.flags, entry);
		} else if (entry->type == ref(EntryType_Socket)) {
			call(OnSocketEvent, event.flags, (void *) entry->data);
		} else {
			assert(false);
		}
	}

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

def(bool, IsRunning) {
	return this->running;
}
