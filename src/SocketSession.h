#import "String.h"
#import "Exception.h"
#import "Connection.h"
#import "SocketConnection.h"
#import "ConnectionInterface.h"

#undef self
#define self SocketSession

enum {
	excNotIdle = excOffset
};

set(ref(OperationType)) {
	ref(OperationType_Idle),
	ref(OperationType_Buffer),
	ref(OperationType_File)
};

DefineCallback(ref(OnDone), void, void *);

class(self) {
	SocketConnection *conn;

	struct {
		ref(OperationType) type;

		union {
			struct {
				size_t offset;
				String s;
				ref(OnDone) onDone;
			} buffer;

			struct {
				File file;
				u64  offset;
				u64  length;
				ref(OnDone) onDone;
			} file;
		};
	} op;
};

def(void, Init, SocketConnection *conn);
def(void, Write, String s, ref(OnDone) onDone);
def(void, SendFile, File file, size_t length, ref(OnDone) onDone);
def(void, Continue);
def(bool, IsIdle);
