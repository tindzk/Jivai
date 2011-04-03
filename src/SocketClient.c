#import "SocketClient.h"

#define self SocketClient

rsdef(self *, New, size_t size) {
	self *res = Pool_Alloc(Pool_GetInstance(), sizeof(self) + size);
	res->conn = NULL;
	return res;
}

def(void, Destroy) {
	if (this->conn != NULL) {
		SocketConnection_Close(this->conn);
		Pool_Free(Pool_GetInstance(), this->conn);
	}

	Pool_Free(Pool_GetInstance(), this);
}

def(void, Accept, Socket *socket) {
	SocketConnection conn = Socket_Accept(socket);

	this->conn =
		SocketConnection_GetObject(
			SocketConnection_Clone(&conn));

	this->conn->corking     = true;
	this->conn->nonblocking = true;
}
