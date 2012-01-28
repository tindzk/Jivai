#import "EventLoop.h"

#define self EventLoop

Singleton(self);
SingletonDestructor(self);

/* Note that this method does not close any file descriptors. Its only purpose
 * is to free the associated memory.
 */
static def(void, destroyEntry, void *addr) {
	ref(Entry) *entry = addr;

	assert(entry->events.onDestroy != null);
	entry->events.onDestroy(entry->events.inst);
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
	/* The registered callback may access the channel watcher. */
	DoublyLinkedList_Destroy(&this->entries,
		LinkedList_OnDestroy_For(this, ref(destroyEntry)));

	ChannelWatcher_Destroy(&this->watcher);

	EventQueue_Destroy(&this->queue);
}

def(void, pullDown, void *object) {
	DoublyLinkedList_safeEach(&this->entries, node) {
		if (node->object == object) {
			DoublyLinkedList_Remove(&this->entries, node);
			call(destroyEntry, node);
		}
	}
}

def(ref(Entry) *, createEntry, void *object, size_t size) {
	ref(Entry) *entry = Memory_New(sizeof(ref(Entry)) + size);

	entry->object = object;
	entry->events = (ref(Events)) { .onInput = null };

	return entry;
}

/* Stop listening to input/output. Must be called when the channel's
 * closure doesn't take effect immediately.
 */
def(void, finalise, ref(Entry) *entry) {
	int flags = ChannelWatcher_Events_Error  |
				ChannelWatcher_Events_HangUp |
				ChannelWatcher_Events_PeerHangUp;

	ChannelWatcher_Modify(&this->watcher, entry->ch, flags, entry);
}

/* This method can only be called once for each entry. */
def(void, attach, ref(Entry) *entry, ref(Options) opts) {
	int flags = ChannelWatcher_Events_Error  |
				ChannelWatcher_Events_HangUp |
				ChannelWatcher_Events_PeerHangUp;

	assert(opts.events.inst.addr != null);
	assert(opts.events.onDestroy != null);

	if (opts.edgeTriggered) {
		BitMask_Set(flags, ChannelWatcher_Events_EdgeTriggered);
	}

	if (opts.events.onInput != null) {
		flags |= ChannelWatcher_Events_Input;
	}

	if (opts.events.onOutput != null) {
		flags |= ChannelWatcher_Events_Output;
	}

	ChannelWatcher_Subscribe(&this->watcher, opts.ch, flags, entry);
	DoublyLinkedList_InsertEnd(&this->entries, entry);

	entry->ch     = opts.ch;
	entry->events = opts.events;
}

/* Set `watcher' to true if the channel continues to be valid.
 * Otherwise the kernel removes the watcher automatically as soon as
 * the channel is closed. In this case we can save one system call.
 * detach() must only be called from the function connected to the
 * `destroy' event: After this method returns, all associated memory
 * will not be accessible any longer.
 */
def(void, detach, ref(Entry) *entry, bool watcher) {
	EventQueue_Prune(&this->queue, entry);

	if (watcher) {
		ChannelWatcher_Unsubscribe(&this->watcher, entry->ch);
	}

	DoublyLinkedList_Remove(&this->entries, entry);

	Memory_Destroy(entry);
}

static inline def(void, _enqueue, void *entry, int events) {
	if (BitMask_Has(events,
			ChannelWatcher_Events_Error  |
			ChannelWatcher_Events_HangUp |
			ChannelWatcher_Events_PeerHangUp))
	{
		/* Hang ups will free resources and must not occur twice
		 * in the event queue.
		 */
		assert(!EventQueue_hasEvent(&this->queue, entry, events));
	}

	EventQueue_Enqueue(&this->queue, entry, events);
}

def(void, enqueue, ref(Entry) *entry, int events) {
	call(_enqueue, entry, events);
}

static def(void, onEvent, int events, ref(Entry) *entry) {
	if (BitMask_Has(events,
			ChannelWatcher_Events_Error  |
			ChannelWatcher_Events_HangUp |
			ChannelWatcher_Events_PeerHangUp))
	{
		/* Error occurred or connection hung up. */
		assert(entry->events.onDestroy != null);
		entry->events.onDestroy(entry->events.inst);
		return;
	}

	if (BitMask_Has(events, ChannelWatcher_Events_Input)) {
		/* Receiving data from client. */
		assert(entry->events.onInput != null);
		entry->events.onInput(entry->events.inst);
	}

	if (BitMask_Has(events, ChannelWatcher_Events_Output)) {
		/* Client requests data. */
		assert(entry->events.onOutput != null);
		entry->events.onOutput(entry->events.inst);
	}
}

def(void, iteration, int timeout) {
	/* Poll for new events and insert into event queue. */
	ChannelWatcher_Poll(&this->watcher,
		ChannelWatcher_OnEvent_For(this, ref(_enqueue)), timeout);

	while (EventQueue_HasEvents(&this->queue)) {
		/* Pop an event and process it. Note that each event can enqueue
		 * further events.
		 */

		EventQueue_Event event = EventQueue_Pop(&this->queue);
		call(onEvent, event.flags, event.ptr);
	}

	callback(this->onTimeout, timeout);
}

overload def(void, run, int timeout) {
	this->running = true;
	while (this->running) {
		call(iteration, timeout);
	}
}

def(void, quit) {
	this->running = false;
}

def(bool, isRunning) {
	return this->running;
}
