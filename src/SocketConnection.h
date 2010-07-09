#ifndef SOCKET_CONNECTION_H
#define SOCKET_CONNECTION_H

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

#include "File.h"
#include "String.h"
#include "Exception.h"
#include "NetworkAddress.h"

#define SocketConnection_ChunkSize 65536

Exception_Export(SocketConnection_ConnectionRefusedException);
Exception_Export(SocketConnection_ConnectionResetException);
Exception_Export(SocketConnection_EmptyQueueException);
Exception_Export(SocketConnection_FcntlFailedException);
Exception_Export(SocketConnection_FileDescriptorUnusableException);
Exception_Export(SocketConnection_InvalidFileDescriptorException);
Exception_Export(SocketConnection_LengthMismatchException);
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
size_t SocketConnection_Read(SocketConnection *this, void *buf, size_t len);
size_t SocketConnection_Write(SocketConnection *this, void *buf, size_t len);
void SocketConnection_SendFile(SocketConnection *this, File *file, off64_t *offset, size_t len);
void SocketConnection_Close(SocketConnection *this);

#endif
