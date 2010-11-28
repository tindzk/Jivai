#import "SocketConnection.h"

Interface(Connection) {
	size_t size;

	void (*init)   (GenericInstance, SocketConnection *);
	void (*destroy)(GenericInstance);
	bool (*pull)   (GenericInstance);
	bool (*push)   (GenericInstance);
};
