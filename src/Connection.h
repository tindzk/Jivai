#import "Logger.h"
#import "SocketConnection.h"

Interface(Connection) {
	size_t size;

	Method(void, init, SocketConnection *, Logger *);
	Method(void, destroy);
	Method(ClientConnection_Status, pull);
	Method(ClientConnection_Status, push);
};
