#import "String.h"
#import "Connection.h"
#import "SocketConnection.h"
#import "ExceptionManager.h"
#import "ConnectionInterface.h"

typedef void (* SocketSession_OnDone)(void *, void *);

Exception_Export(SocketSession_NotIdleException);

typedef enum {
	SocketSession_OperationType_Idle,
	SocketSession_OperationType_Buffer,
	SocketSession_OperationType_File
} SocketSession_OperationType;

typedef struct _SocketSession {
	Connection_Define(_SocketSession);

	void *context;
	SocketConnection *conn;

	struct {
		SocketSession_OperationType type;

		union {
			struct {
				size_t offset;
				String buffer;
				SocketSession_OnDone onDone;
			} buffer;

			struct {
				File    file;
				off64_t offset;
				size_t  length;
				SocketSession_OnDone onDone;
			} file;
		} u;
	} operation;
} SocketSession;

void SocketSession0(ExceptionManager *e);
void SocketSession_Init(SocketSession *this, SocketConnection *conn, void *context);
void SocketSession_Write(SocketSession *this, String s, SocketSession_OnDone onDone);
void SocketSession_SendFile(SocketSession *this, File file, size_t length, SocketSession_OnDone onDone);
void SocketSession_Continue(SocketSession *this);
bool SocketSession_IsIdle(SocketSession *this);
