#import <errno.h>
#import <netdb.h>

#import "String.h"
#import "Exception.h"

#undef self
#define self NetworkAddress

enum {
	excGetAddrInfoFailed = excOffset
};

typedef struct {
	unsigned long ip;
	unsigned short port;
} NetworkAddress;

void NetworkAddress0(void *e);

struct in_addr NetworkAddress_ResolveHost(String hostname);
String NetworkAddress_ToString(NetworkAddress addr);
