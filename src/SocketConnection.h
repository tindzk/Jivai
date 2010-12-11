#import <errno.h>
#import <linux/net.h>
#import <netinet/tcp.h>

#import "File.h"
#import "Kernel.h"
#import "String.h"
#import "Exception.h"
#import "NetworkAddress.h"

#define self SocketConnection

#ifndef SocketConnection_ChunkSize
#define SocketConnection_ChunkSize 65536
#endif

// @exc ConnectionRefused
// @exc ConnectionReset
// @exc FcntlFailed
// @exc FileDescriptorUnusable
// @exc InvalidFileDescriptor
// @exc NotConnected
// @exc UnknownError

class {
	NetworkAddress addr;

	ssize_t fd;

	bool corking;
	bool closable;
	bool nonblocking;
};

def(void, Flush);
def(ssize_t, Read, void *buf, size_t len);
def(bool, SendFile, File *file, u64 *offset, size_t len);
def(ssize_t, Write, void *buf, size_t len);
def(void, Close);

#undef self
