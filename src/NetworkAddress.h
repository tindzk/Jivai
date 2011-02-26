#import <errno.h>
#import <netdb.h>

#import "String.h"
#import "Exception.h"

#define self NetworkAddress

// @exc GetAddrInfoFailed

record(self) {
	unsigned long ip;
	unsigned short port;
};

sdef(struct in_addr, ResolveHost, ProtString hostname);
sdef(String, ToString, NetworkAddress addr);

#undef self
