#import <errno.h>
#import <netdb.h>

#import "String.h"
#import "Exception.h"

#define self NetworkAddress

enum {
	excGetAddrInfoFailed = excOffset
};

record(self) {
	unsigned long ip;
	unsigned short port;
};

struct in_addr NetworkAddress_ResolveHost(String hostname);
String NetworkAddress_ToString(NetworkAddress addr);

#undef self
