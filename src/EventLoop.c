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

def(void, AddTimer, int sec, ref(OnTimer) onTimer) {
	ref(Entry) *entry = Pool_Alloc(Pool_GetInstance(),
		sizeof(ref(Entry)) + sizeof(ref(TimerEntry)));

	entry->type = ref(EntryType_Timer);

	ref(TimerEntry) *data = (void *) entry->data;

	data->timer = Timer_New(ClockType_Monotonic);
	data->cb    = onTimer;

	Timer_SetTimer(&data->timer, sec);

	Poll_AddFd(&this->poll, entry, data->timer.fd, Poll_Events_Input);

	DoublyLinkedList_InsertEnd(&this->entries, entry);
}

def(void, AddIntervalTimer, int sec, ref(OnTimer) onTimer) {
	ref(Entry) *entry = Pool_Alloc(Pool_GetInstance(),
		sizeof(ref(Entry)) + sizeof(ref(TimerEntry)));

	entry->type = ref(EntryType_Timer);

	ref(TimerEntry) *data = (void *) entry->data;

	data->timer = Timer_New(ClockType_Monotonic);
	data->cb    = onTimer;

	Timer_SetInterval(&data->timer, sec);

	Poll_AddFd(&this->poll, entry, data->timer.fd, Poll_Events_Input);

	DoublyLinkedList_InsertEnd(&this->entries, entry);
}

/* Listens for incoming connections. */
def(void, AttachSocket, Socket *socket, ref(OnConnection) onConnection) {
	ref(Entry) *entry = Pool_Alloc(Pool_GetInstance(),
		sizeof(ref(Entry)) + sizeof(ref(SocketEntry)));

	entry->type = ref(EntryType_Socket);

	ref(SocketEntry) *data = (void *) entry->data;

	data->cb     = onConnection;
	data->socket = socket;

	Poll_AddFd(&this->poll, entry, socket->fd,
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

	Poll_AddFd(&this->poll, entry, data->conn.fd, flags);

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

static def(void, _DetachTimer, ref(Entry) *entry) {
	ref(TimerEntry) *data = (void *) entry->data;

	Timer_Destroy(&data->timer);

	DoublyLinkedList_Remove(&this->entries, entry);

	call(_DestroyEntry, entry);
}

/* Users can only detach interval timers as normal timers are detached
 * automatically.
 */
def(void, DetachTimer, Timer *timer) {
	/* This assumes that `timer' resides in the first element of TimerEntry. */
	ref(Entry) *entry = (void *) timer - sizeof(ref(Entry));

	assert(entry->type == ref(EntryType_Timer));
	assert(Timer_IsInterval(timer));

	call(_DetachTimer, entry);
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

static def(void, OnTimerEvent, int events, ref(Entry) *entry) {
	ref(TimerEntry) *data = (void *) entry->data;

	if (BitMask_Has(events, Poll_Events_Input)) {
		/* Store value locally because the callback could have detached the
		 * timer in the meantime.
		 */
		bool interval = Timer_IsInterval(&data->timer);

		callback(data->cb, Timer_Read(&data->timer), &data->timer);

		if (!interval) {
			/* The timer is not recurring. Thus, we have to detach it. */
			call(_DetachTimer, entry);
		}
	}
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

	if (entry->type == ref(EntryType_Timer)) {
		call(OnTimerEvent, events, entry);
	} else if (entry->type == ref(EntryType_Client)) {
		call(OnClientEvent, events, entry);
	} else if (entry->type == ref(EntryType_Socket)) {
		call(OnSocketEvent, events, (void *) entry->data);
	}
}
