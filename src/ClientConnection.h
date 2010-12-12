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
};

DoublyLinkedList_DeclareList(self, ClientConnections);

#undef self
