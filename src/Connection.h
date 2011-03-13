#import "Logger.h"
#import "SocketConnection.h"

Interface(Connection) {
	size_t size;

	Method(void, init, SocketConnection *, Logger *);
	Method(void, destroy);
	Method(bool, pull);
	Method(bool, push);
};
