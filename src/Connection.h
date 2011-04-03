#import "Logger.h"
#import "SocketConnection.h"

#define self Connection

set(ref(Status)) {
	ref(Status_Close),
	ref(Status_Open)
};

Interface(self) {
	size_t size;

	void        (*init)   (GenericInstance, SocketConnection *, Logger *);
	void        (*destroy)(GenericInstance);
	ref(Status) (*pull)   (GenericInstance);
	ref(Status) (*push)   (GenericInstance);
};

#undef self
