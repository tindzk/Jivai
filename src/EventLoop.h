#import "Poll.h"
#import "Timer.h"
#import "Socket.h"
#import "DoublyLinkedList.h"

#define self EventLoop

Interface(ref(Client)) {
	size_t (*getSize)  (GenericInstance);
	void   (*onInput)  (GenericInstance, GenericInstance inst);
	void   (*onOutput) (GenericInstance, GenericInstance inst);
	void   (*onDestroy)(GenericInstance, GenericInstance inst);
};

Callback(ref(OnTimer),      void, u64 dropped, Timer *timer);
Callback(ref(OnTimeout),    void, int timeout);
Callback(ref(OnConnection), void, Socket *socket);

record(ref(TimerEntry)) {
	Timer timer;
	ref(OnTimer) cb;
};

record(ref(SocketEntry)) {
	Socket *socket;
	ref(OnConnection) cb;
};

record(ref(ClientEntry)) {
	SocketConnection conn;
	ref(Client)      client;
	char             data[];
};

set(ref(EntryType)) {
	ref(EntryType_Timer),
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
	Poll poll;
	bool running;
	ref(Entries) entries;
	ref(OnTimeout) onTimeout;
};

def(void, Init);
def(void, Destroy);
def(void, AddTimer, int sec, ref(OnTimer) onTimer);
def(void, AddIntervalTimer, int sec, ref(OnTimer) onTimer);
def(void, AttachSocket, Socket *socket, ref(OnConnection) onConnection);
def(ref(ClientEntry) *, AcceptClient, Socket *socket, bool edgeTriggered, ref(Client) client);
def(void, DetachClient, GenericInstance inst);
def(void, DetachTimer, Timer *timer);
def(bool, IsRunning);
def(void, Iteration, int timeout);
overload def(void, Run, int timeout);
def(void, Quit);

static alwaysInline overload def(void, Run) {
	call(Run, -1);
}

SingletonPrototype(self);

#undef self
