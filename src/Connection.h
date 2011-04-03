#import "Logger.h"
#import "ClientConnection.h"
#import "SocketConnection.h"

Interface(Connection) {
	size_t size;

	void (*init)   (GenericInstance, SocketConnection *, Logger *);
	void (*destroy)(GenericInstance);
	ClientConnection_Status (*pull) (GenericInstance);
	ClientConnection_Status (*push) (GenericInstance);
};
