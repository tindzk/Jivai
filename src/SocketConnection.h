#import <errno.h>
#import <linux/net.h>
#import <netinet/tcp.h>

#import "File.h"
#import "Fcntl.h"
#import "String.h"
#import "Exception.h"
#import "NetworkAddress.h"

#undef self
#define self SocketConnection

#ifndef SocketConnection_ChunkSize
#define SocketConnection_ChunkSize 65536
#endif

enum {
	excConnectionRefused = excOffset,
	excFileDescriptorUnusable,
	excNotConnected
};

typedef struct {
	NetworkAddress addr;

	ssize_t fd;

	bool corking;
	bool closable;
	bool nonblocking;
} SocketConnection;

void SocketConnection0(ExceptionManager *e);

void SocketConnection_Flush(SocketConnection *this);
ssize_t SocketConnection_Read(SocketConnection *this, void *buf, size_t len);
ssize_t SocketConnection_Write(SocketConnection *this, void *buf, size_t len);
bool SocketConnection_SendFile(SocketConnection *this, File *file, u64 *offset, size_t len);
void SocketConnection_Close(SocketConnection *this);
