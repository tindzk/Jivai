#import <errno.h>
#import <netdb.h>

#import "ExceptionManager.h"

#undef self
#define self NetworkAddress

enum {
	excGetAddrInfoFailed = excOffset
};

extern size_t Modules_NetworkAddress;

typedef struct {
	unsigned long ip;
	unsigned short port;
} NetworkAddress;

void NetworkAddress0(ExceptionManager *e);

struct in_addr NetworkAddress_ResolveHost(String hostname);
String NetworkAddress_ToString(NetworkAddress addr);
