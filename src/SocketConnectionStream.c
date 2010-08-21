#import "SocketConnectionStream.h"

bool SocketConnectionStream_IsEof(__unused SocketConnectionStream *this) {
	return false;
}

StreamInterface SocketConnectionStream_Methods = {
	.read  = (void *) SocketConnectionStream_Read,
	.write = (void *) SocketConnectionStream_Write,
	.close = (void *) SocketConnectionStream_Close,
	.isEof = (void *) SocketConnectionStream_IsEof,
};
