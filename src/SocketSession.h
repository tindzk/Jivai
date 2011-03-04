#import "String.h"
#import "Exception.h"
#import "SocketConnection.h"

#define self SocketSession

// @exc NotIdle

set(ref(OperationType)) {
	ref(OperationType_Idle),
	ref(OperationType_Buffer),
	ref(OperationType_File)
};

DefineCallback(ref(OnDone), void, void *);

class {
	SocketConnection *conn;

	struct {
		ref(OperationType) type;

		union {
			struct {
				size_t offset;
				ProtString s;
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

rsdef(self, New, SocketConnection *conn);
def(void, Write, ProtString s, ref(OnDone) onDone);
def(void, SendFile, File file, u64 length, ref(OnDone) onDone);
def(void, Continue);
def(void, Flush);
def(bool, IsIdle);

#undef self
