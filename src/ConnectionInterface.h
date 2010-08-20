#import "SocketConnection.h"

typedef struct {
	void* (*new)();
	void (*init)(void *, SocketConnection *);
	void (*destroy)(void *);
	bool (*pull)(void *);
	bool (*push)(void *);
} ConnectionInterface;
