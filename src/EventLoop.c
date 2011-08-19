#import "EventLoop.h"

#define self EventLoop

Singleton(self);
SingletonDestructor(self);

/* Note that this method does not close any file descriptors. Its only purpose
 * is to free the associated memory.
 */
static def(void, _DestroyEntry, void *addr) {
	ref(Entry) *entry = addr;

	if (entry->type == ref(EntryType_Client)) {
		ref(ClientEntry) *data = (void *) entry->data;
		delegate(data->client, onDestroy, data->data);
	}

	Memory_Destroy(entry);
}

rsdef(self, New) {
	return (self) {
		.onTimeout = scall(OnTimeout_Empty),
		.watcher   = ChannelWatcher_New(),
		.queue     = EventQueue_New(),
		.entries   = DoublyLinkedList_New()
	};
}

def(void, Destroy) {
	ChannelWatcher_Destroy(&this->watcher);

	EventQueue_Destroy(&this->queue);

	DoublyLinkedList_Destroy(&this->entries,
		LinkedList_OnDestroy_For(this, ref(_DestroyEntry)));
}

def(void, pullDown, void *object) {
	DoublyLinkedList_safeEach(&this->entries, node) {
		if (node->object == object) {
			DoublyLinkedList_Remove(&this->entries, node);
			call(_DestroyEntry, node);
		}
	}
}

def(ref(Entry) *, AddChannel, void *object, Channel *ch, ref(OnInput) onInput, ref(OnOutput) onOutput, ref(OnDestroy) onDestroy) {
	ref(Entry) *entry = Memory_New(sizeof(ref(Entry)) + sizeof(ref(ChannelEntry)));

	entry->type   = ref(EntryType_Channel);
	entry->object = object;

	ref(ChannelEntry) *data = (void *) entry->data;

	data->ch    = ch;
	data->cbIn  = onInput;
	data->cbOut = onOutput;
	data->cbDestroy = onDestroy;

	int flags = 0;

	if (hasCallback(onInput)) {
		flags |= ChannelWatcher_Events_Input;
	}

	if (hasCallback(onOutput)) {
		flags |= ChannelWatcher_Events_Output;
	}

	if (hasCallback(onDestroy)) {
		flags |= ChannelWatcher_Events_Error;
		flags |= ChannelWatcher_Events_HangUp;
		flags |= ChannelWatcher_Events_PeerHangUp;
	}

	ChannelWatcher_Subscribe(&this->watcher, data->ch, flags, entry);
	DoublyLinkedList_InsertEnd(&this->entries, entry);

	return entry;
}

/* Set `watcher' to true if the fd continues to be valid. Otherwise the kernel
 * removes the watcher automatically upon closure.
 */
def(void, DetachChannel, ref(Entry) *entry, bool watcher) {
	EventQueue_Prune(&this->queue, entry);

	ref(ChannelEntry) *data = (void *) entry->data;

	assert(entry->type == ref(EntryType_Channel));

	if (watcher) {
		ChannelWatcher_Unsubscribe(&this->watcher, data->ch);
	}

	DoublyLinkedList_Remove(&this->entries, entry);

	call(_DestroyEntry, entry);
}

/* Listens for incoming connections. */
def(void, AttachSocket, void *object, SocketServer *socket, ref(OnConnection) onConnection) {
	ref(Entry) *entry = Memory_New(sizeof(ref(Entry)) + sizeof(ref(SocketEntry)));

	entry->type   = ref(EntryType_Socket);
	entry->object = object;

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
def(ref(ClientEntry) *, AcceptClient, void *object, SocketServer *socket, bool edgeTriggered, ref(Client) client) {
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

	ref(Entry) *entry = Memory_New(
		sizeof(ref(Entry))       +
		sizeof(ref(ClientEntry)) +
		delegate(client, getSize));

	entry->type   = ref(EntryType_Client);
	entry->object = object;

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
	EventQueue_Prune(&this->queue, entry);

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
def(void, DetachClient, void *addr) {
	ref(Entry) *entry = addr
		- sizeof(ref(ClientEntry))
		- sizeof(ref(Entry));

	call(_DetachClient, entry);
}

static def(void, OnChannelEvent, int events, ref(Entry) *entry) {
	ref(ChannelEntry) *data = (void *) entry->data;

	if (BitMask_Has(events,
			ChannelWatcher_Events_Error  |
			ChannelWatcher_Events_HangUp |
			ChannelWatcher_Events_PeerHangUp))
	{
		/* Error occurred or connection hung up. */
		callback(data->cbDestroy);
		return;
	}

	if (BitMask_Has(events, ChannelWatcher_Events_Input)) {
		callback(data->cbIn);
	}

	if (BitMask_Has(events, ChannelWatcher_Events_Output)) {
		callback(data->cbOut);
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

def(void, Enqueue, void *addr, int events) {
	EventQueue_Enqueue(&this->queue, addr, events);
}

def(void, ClientEnqueue, void *addr, int events) {
	ref(Entry) *entry = addr
		- sizeof(ref(ClientEntry))
		- sizeof(ref(Entry));

	assert(entry->type == ref(EntryType_Client));

	call(Enqueue, entry, events);
}

def(void, Iteration, int timeout) {
	/* Poll for new events and insert into event queue. */
	ChannelWatcher_Poll(&this->watcher,
		ChannelWatcher_OnEvent_For(this, ref(Enqueue)), timeout);

	while (EventQueue_HasEvents(&this->queue)) {
		/* Pop an event and process it. Note that each event can enqueue further
		 * events.
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
