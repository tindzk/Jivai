#ifndef CONNECTION_INTERFACE_H
#define CONNECTION_INTERFACE_H

#include "SocketConnection.h"

typedef struct {
	void* (*new)();
	void (*init)(void *, SocketConnection *);
	void (*destroy)(void *);
	bool (*process)(void *);
} ConnectionInterface;

#endif
