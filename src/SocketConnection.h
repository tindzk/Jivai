#import <linux/net.h>
#import <netinet/tcp.h>

#import "File.h"
#import "Kernel.h"
#import "String.h"
#import "Channel.h"
#import "Exception.h"
#import "NetworkAddress.h"

#define self SocketConnection

#ifndef SocketConnection_ChunkSize
#define SocketConnection_ChunkSize 65536
#endif

// @exc ConnectionRefused
// @exc ConnectionReset
// @exc FileDescriptorUnusable
// @exc InvalidFileDescriptor
// @exc NotConnected
// @exc UnknownError

class {
	Channel ch;
	NetworkAddress addr;
	bool corking;
};

rsdef(self, New, Channel *ch, NetworkAddress addr);
def(void, Destroy);
def(void, SetCorking, bool value);
def(void, Flush);
def(ssize_t, Read, void *buf, size_t len);
def(bool, SendFile, File *file, u64 *offset, size_t len);
overload def(ssize_t, Write, void *buf, size_t len);

static alwaysInline def(Channel *, GetChannel) {
	return &this->ch;
}

static alwaysInline def(NetworkAddress, GetAddress) {
	return this->addr;
}

static alwaysInline overload def(ssize_t, Write, RdString s) {
	return call(Write, s.buf, s.len);
}

#undef self
