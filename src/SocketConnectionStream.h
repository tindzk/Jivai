#ifndef SOCKET_CONNECTION_STREAM_H
#define SOCKET_CONNECTION_STREAM_H

#include "StreamInterface.h"
#include "SocketConnection.h"

#define SocketConnectionStream       SocketConnection
#define SocketConnectionStream_Read  SocketConnection_Read
#define SocketConnectionStream_Write SocketConnection_Write
#define SocketConnectionStream_Close SocketConnection_Close

bool SocketConnectionStream_IsEof(SocketConnectionStream *this);

extern StreamInterface SocketConnectionStream_Methods;

#endif
