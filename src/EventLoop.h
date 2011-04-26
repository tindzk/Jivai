#import "Channel.h"
#import "SocketServer.h"
#import "ChannelWatcher.h"
#import "DoublyLinkedList.h"
#import "SocketConnection.h"

#define self EventLoop

Interface(ref(Client)) {
	size_t (*getSize)  (GenericInstance);
	void   (*onInput)  (GenericInstance, GenericInstance inst);
	void   (*onOutput) (GenericInstance, GenericInstance inst);
	void   (*onDestroy)(GenericInstance, GenericInstance inst);
};

Callback(ref(OnInput),      void);
Callback(ref(OnTimeout),    void, int timeout);
Callback(ref(OnConnection), void, SocketServer *socket);

record(ref(ChannelEntry)) {
	Channel *ch;
	ref(OnInput) cb;
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
	ref(EntryType) type;
	char data[];
};

DoublyLinkedList_DeclareList(ref(Entry), ref(Entries));

class {
	ChannelWatcher watcher;
	bool running;
	ref(Entries) entries;
	ref(OnTimeout) onTimeout;
};

def(void, Init);
def(void, Destroy);
def(ref(Entry) *, AddChannel, Channel *ch, ref(OnInput) onInput);
def(void, DetachChannel, ref(Entry) *entry, bool watcher);
def(void, AttachSocket, SocketServer *socket, ref(OnConnection) onConnection);
def(ref(ClientEntry) *, AcceptClient, SocketServer *socket, bool edgeTriggered, ref(Client) client);
def(void, DetachClient, GenericInstance inst);
def(bool, IsRunning);
def(void, Iteration, int timeout);
overload def(void, Run, int timeout);
def(void, Quit);

static alwaysInline overload def(void, Run) {
	call(Run, -1);
}

SingletonPrototype(self);

#undef self
