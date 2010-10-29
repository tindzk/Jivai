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

class(self) {
	NetworkAddress addr;

	ssize_t fd;

	bool corking;
	bool closable;
	bool nonblocking;
};

void SocketConnection0(ExceptionManager *e);

def(void, Flush);
def(ssize_t, Read, void *buf, size_t len);
def(bool, SendFile, File *file, u64 *offset, size_t len);
def(ssize_t, Write, void *buf, size_t len);
def(void, Close);
