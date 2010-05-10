#ifndef STREAM_INTERFACE_H
#define STREAM_INTERFACE_H

#include <stddef.h>

typedef struct {
	size_t (*read)(void *, void *, size_t);
	size_t (*write)(void *, void *, size_t);
	void (*close)(void *);
} StreamInterface;

#endif
