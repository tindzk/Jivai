#import "StreamInterface.h"
#import "SocketConnection.h"

#undef self
#define self SocketConnectionStream

#define SocketConnectionStream       SocketConnection
#define SocketConnectionStream_Read  SocketConnection_Read
#define SocketConnectionStream_Write SocketConnection_Write
#define SocketConnectionStream_Close SocketConnection_Close

def(bool, IsEof);

StreamInterface Impl(self);
