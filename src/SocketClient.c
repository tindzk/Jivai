#import "SocketClient.h"

#define self SocketClient

def(void, Init) {
	this->conn = NULL;
	this->data = NULL;
}

def(void, Destroy) {
	if (this->conn != NULL) {
		SocketConnection_Close(this->conn);
		Pool_Free(Pool_GetInstance(), this->conn);
	}
}

def(void, SetData, void *data) {
	this->data = data;
}

def(void *, GetData) {
	return this->data;
}

def(SocketConnection *, GetConn) {
	return this->conn;
}

def(ssize_t, GetFd) {
	return this->conn->fd;
}

def(void, Accept, Socket *socket) {
	SocketConnection conn = Socket_Accept(socket);

	this->conn =
		SocketConnection_GetObject(
			SocketConnection_Clone(&conn));

	this->conn->corking     = true;
	this->conn->nonblocking = true;
}
