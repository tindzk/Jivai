#import "SocketClient.h"
#import "DoublyLinkedList.h"

#define self ClientConnection

set(ref(Status)) {
	ref(Status_Close),
	ref(Status_Open)
};

class {
	SocketClient *client;
	DoublyLinkedList_DeclareRef(self);
	char data[];
};

static alwaysInline rsdef(self *, Alloc, size_t dataSize) {
	return Pool_Alloc(Pool_GetInstance(), sizeof(self) + dataSize);
}

static alwaysInline def(void *, GetData) {
	return (void *) &this->data;
}

DoublyLinkedList_DeclareList(self, ClientConnections);

#undef self
