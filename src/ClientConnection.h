#import "SocketClient.h"
#import "DoublyLinkedList.h"

#define self ClientConnection

set(ref(Status)) {
	ref(Status_Close),
	ref(Status_Open)
};

#undef self
