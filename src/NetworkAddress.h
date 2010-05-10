#ifndef NETWORK_ADDRESS_H
#define NETWORK_ADDRESS_H

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "ExceptionManager.h"

Exception_Export(NetworkAddress_GetAddrInfoFailedException);

typedef struct {
	unsigned long ip;
	unsigned short port;
} NetworkAddress;

void NetworkAddress0(ExceptionManager *e);

struct in_addr NetworkAddress_ResolveHost(String hostname);
String NetworkAddress_ToString(NetworkAddress addr);

#endif
