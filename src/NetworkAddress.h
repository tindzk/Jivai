#import <errno.h>
#import <netdb.h>

#import "ExceptionManager.h"

#undef self
#define self NetworkAddress

Exception_Export(GetAddrInfoFailedException);

typedef struct {
	unsigned long ip;
	unsigned short port;
} NetworkAddress;

void NetworkAddress0(ExceptionManager *e);

struct in_addr NetworkAddress_ResolveHost(String hostname);
String NetworkAddress_ToString(NetworkAddress addr);
