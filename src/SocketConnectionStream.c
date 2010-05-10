#include "SocketConnectionStream.h"

StreamInterface SocketConnectionStream_Methods = {
	.read  = (void *) SocketConnectionStream_Read,
	.write = (void *) SocketConnectionStream_Write,
	.close = (void *) SocketConnectionStream_Close
};
