#ifndef SOCKET_H
#define SOCKET_H

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "String.h"
#include "NetworkAddress.h"
#include "SocketConnection.h"
#include "ExceptionManager.h"

typedef enum {
	Socket_Protocol_TCP,
	Socket_Protocol_UDP
} Socket_Protocol;

Exception_Export(Socket_AcceptFailedException);
Exception_Export(Socket_AddressInUseException);
Exception_Export(Socket_BindFailedException);
Exception_Export(Socket_ConnectFailedException);
Exception_Export(Socket_FcntlFailedException);
Exception_Export(Socket_ListenFailedException);
Exception_Export(Socket_SetSocketOptionException);
Exception_Export(Socket_SocketFailedException);

typedef struct {
	int fd;
} Socket;

void Socket0(ExceptionManager *e);

void Socket_Init(Socket *this, Socket_Protocol protocol);
void Socket_SetNonBlockingFlag(Socket *this, bool enable);
void Socket_SetCloexecFlag(Socket *this, bool enable);
void Socket_SetReusableFlag(Socket *this);
void Socket_Listen(Socket *this, unsigned short port, int maxconns);
void Socket_SetLinger(Socket *this);
SocketConnection Socket_Connect(Socket *this, String hostname, unsigned short port);
SocketConnection Socket_Accept(Socket *this);
void Socket_Destroy(Socket *this);

#endif
