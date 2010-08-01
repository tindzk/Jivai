#ifndef SOCKET_CONNECTION_H
#define SOCKET_CONNECTION_H

#include <errno.h>
#include <linux/net.h>
#include <netinet/tcp.h>

#include "File.h"
#include "Fcntl.h"
#include "String.h"
#include "Exception.h"
#include "NetworkAddress.h"

#ifndef SocketConnection_ChunkSize
#define SocketConnection_ChunkSize 65536
#endif

Exception_Export(SocketConnection_ConnectionRefusedException);
Exception_Export(SocketConnection_ConnectionResetException);
Exception_Export(SocketConnection_FcntlFailedException);
Exception_Export(SocketConnection_FileDescriptorUnusableException);
Exception_Export(SocketConnection_InvalidFileDescriptorException);
Exception_Export(SocketConnection_NotConnectedException);
Exception_Export(SocketConnection_UnknownErrorException);

typedef struct {
	NetworkAddress addr;

	int fd;

	bool corking;
	bool closable;
	bool nonblocking;
} SocketConnection;

void SocketConnection0(ExceptionManager *e);

void SocketConnection_Flush(SocketConnection *this);
ssize_t SocketConnection_Read(SocketConnection *this, void *buf, size_t len);
ssize_t SocketConnection_Write(SocketConnection *this, void *buf, size_t len);
bool SocketConnection_SendFile(SocketConnection *this, File *file, off64_t *offset, size_t len);
void SocketConnection_Close(SocketConnection *this);

#endif
