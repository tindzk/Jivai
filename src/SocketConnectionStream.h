#import "Stream.h"
#import "SocketConnection.h"

#define self SocketConnectionStream

#define SocketConnectionStream       SocketConnection
#define SocketConnectionStream_Read  SocketConnection_Read
#define SocketConnectionStream_Write SocketConnection_Write
#define SocketConnectionStream_Close SocketConnection_Close

def(bool, IsEof);

ExportImpl(Stream);

#undef self
