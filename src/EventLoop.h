#import "Channel.h"
#import "EventQueue.h"
#import "ChannelWatcher.h"
#import "DoublyLinkedList.h"

#define self EventLoop

Callback(ref(OnTimeout), void, int timeout);

record(ref(Events)) {
	/* Compulsory. */
	Instance inst;

	/* Compulsory. Must call EventLoop_detach(). */
	void (*onDestroy)(Instance $this);

	/* Optional. */
	void (*onInput)(Instance $this);

	/* Optional. */
	void (*onOutput)(Instance $this);
};

record(ref(Options)) {
	Channel *ch;
	bool edgeTriggered;
	ref(Events) events;
};

record(ref(Entry)) {
	DoublyLinkedList_DeclareRef(ref(Entry));

	void        *object;
	Channel     *ch;
	ref(Events) events;
	char        data[];
};

DoublyLinkedList_DeclareList(ref(Entry), ref(Entries));

class {
	bool           running;
	EventQueue     queue;
	ChannelWatcher watcher;
	ref(Entries)   entries;
	ref(OnTimeout) onTimeout;
};

rsdef(self, New);
def(void, Destroy);
def(void, pullDown, void *object);
def(ref(Entry) *, createEntry, void *object, size_t size);
def(void, attach, ref(Entry) *entry, ref(Options) opts);
def(void, detach, ref(Entry) *entry, bool watcher);
def(void, enqueue, ref(Entry) *entry, int events);
def(void, iteration, int timeout);
overload def(void, run, int timeout);
def(void, quit);
def(bool, isRunning);

static alwaysInline overload def(void, run) {
	call(run, -1);
}

SingletonPrototype(self);

#undef self
