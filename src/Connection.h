#import "Client.h"
#import "DoublyLinkedList.h"

#undef self
#define self Connection

set(ref(Status)) {
	ref(Status_Close),
	ref(Status_Open)
};

record(self) {
	Client *client;
	DoublyLinkedList_DeclareRef(self);
};

DoublyLinkedList_DeclareList(self, Connections);
