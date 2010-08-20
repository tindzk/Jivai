#import <errno.h>
#import <linux/net.h>
#import <sys/syscall.h>

#import "Fcntl.h"
#import "String.h"
#import "NetworkAddress.h"
#import "SocketConnection.h"
#import "ExceptionManager.h"

#undef self
#define self Socket

typedef enum {
	Socket_Protocol_TCP,
	Socket_Protocol_UDP
} Socket_Protocol;

Exception_Export(AcceptFailedException);
Exception_Export(AddressInUseException);
Exception_Export(BindFailedException);
Exception_Export(ConnectFailedException);
Exception_Export(FcntlFailedException);
Exception_Export(ListenFailedException);
Exception_Export(SetSocketOptionException);
Exception_Export(SocketFailedException);

typedef struct {
	int fd;
	bool unused;
	Socket_Protocol protocol;
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
