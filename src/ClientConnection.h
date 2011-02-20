#import "SocketClient.h"
#import "DoublyLinkedList.h"

#define self ClientConnection

set(ref(Status)) {
	ref(Status_Close),
	ref(Status_Open)
};

record(self) {
	SocketClient *client;
	DoublyLinkedList_DeclareRef(self);
	char data[];
};

static inline self* ClientConnection_New(size_t dataSize) {
	return Pool_Alloc(Pool_GetInstance(), sizeof(self) + dataSize);
}

static inline void* ClientConnection_GetData(self *$this) {
	return (void *) &$this->data;
}

DoublyLinkedList_DeclareList(self, ClientConnections);

#undef self
