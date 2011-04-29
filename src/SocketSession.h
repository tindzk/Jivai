#import "String.h"
#import "Exception.h"
#import "SocketConnection.h"

#define self SocketSession

set(ref(OperationType)) {
	ref(OperationType_Idle),
	ref(OperationType_Buffer),
	ref(OperationType_File)
};

Callback(ref(OnDone), void, void *);

class {
	SocketConnection *conn;

	struct {
		ref(OperationType) type;

		union {
			struct {
				size_t offset;
				RdString s;
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
def(void, Write, RdString s, ref(OnDone) onDone);
def(void, SendFile, File file, u64 length, ref(OnDone) onDone);
def(void, Continue);

static inline def(bool, IsIdle) {
	return this->op.type == ref(OperationType_Idle);
}

static inline def(void, Flush) {
	SocketConnection_Flush(this->conn);
}

#undef self
