#import "Socket.h"
#import "DoublyLinkedList.h"

#define self SocketClient

class {
	DoublyLinkedList_DeclareRef(self);
	SocketConnection *conn;
	char object[];
};

DoublyLinkedList_DeclareList(self, SocketClients);

rsdef(self *, New);
def(void, Destroy);
def(void, Accept, Socket *socket);

#undef self
