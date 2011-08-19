#import "Channel.h"
#import "EventQueue.h"
#import "SocketServer.h"
#import "ChannelWatcher.h"
#import "DoublyLinkedList.h"
#import "SocketConnection.h"

#define self EventLoop

Interface(ref(Client)) {
	size_t (*getSize)  (Instance $this);
	void   (*onInput)  (Instance $this, Instance inst);
	void   (*onOutput) (Instance $this, Instance inst);
	void   (*onDestroy)(Instance $this, Instance inst);
};

Callback(ref(OnInput),      void);
Callback(ref(OnOutput),     void);
Callback(ref(OnDestroy),    void);
Callback(ref(OnTimeout),    void, int timeout);
Callback(ref(OnConnection), void, SocketServer *socket);

record(ref(ChannelEntry)) {
	Channel *ch;

	ref(OnInput)   cbIn;
	ref(OnOutput)  cbOut;
	ref(OnDestroy) cbDestroy;
};

record(ref(SocketEntry)) {
	SocketServer *socket;
	ref(OnConnection) cb;
};

record(ref(ClientEntry)) {
	SocketConnection conn;
	ref(Client)      client;
	char             data[];
};

set(ref(EntryType)) {
	ref(EntryType_Channel),
	ref(EntryType_Socket),
	ref(EntryType_Client)
};

record(ref(Entry)) {
	DoublyLinkedList_DeclareRef(ref(Entry));
	void *object;
	ref(EntryType) type;
	char data[];
};

DoublyLinkedList_DeclareList(ref(Entry), ref(Entries));

class {
	EventQueue queue;
	ChannelWatcher watcher;
	bool running;
	ref(Entries) entries;
	ref(OnTimeout) onTimeout;
};

rsdef(self, New);
def(void, Destroy);
def(void, pullDown, void *object);
def(ref(Entry) *, AddChannel, void *object, Channel *ch, ref(OnInput) onInput, ref(OnOutput) onOutput, ref(OnDestroy) onDestroy);
def(void, DetachChannel, ref(Entry) *entry, bool watcher);
def(void, AttachSocket, void *object, SocketServer *socket, ref(OnConnection) onConnection);
def(ref(ClientEntry) *, AcceptClient, void *object, SocketServer *socket, bool edgeTriggered, ref(Client) client);
def(void, DetachClient, void *addr);
def(void, Enqueue, void *addr, int events);
def(void, ClientEnqueue, void *addr, int events);
def(void, Iteration, int timeout);
overload def(void, Run, int timeout);
def(void, Quit);
def(bool, IsRunning);

static alwaysInline overload def(void, Run) {
	call(Run, -1);
}

SingletonPrototype(self);

#undef self
