#import "SocketSession.h"

#define self SocketSession

def(void, Init, SocketConnection *conn) {
	this->conn    = conn;
	this->op.type = ref(OperationType_Idle);
}

def(void, Write, String s, ref(OnDone) onDone) {
	if (!call(IsIdle)) {
		throw(NotIdle);
	}

	if (s.len == 0) {
		callback(onDone, &s);
		return;
	}

	this->op.type = ref(OperationType_Buffer);

	this->op.buffer.s      = s;
	this->op.buffer.offset = 0;
	this->op.buffer.onDone = onDone;

	call(Continue);
}

def(void, SendFile, File file, u64 length, ref(OnDone) onDone) {
	if (!call(IsIdle)) {
		throw(NotIdle);
	}

	this->op.type = ref(OperationType_File);

	this->op.file.offset = 0;
	this->op.file.file   = file;
	this->op.file.length = length;
	this->op.file.onDone = onDone;

	call(Continue);
}

def(void, Continue) {
	switch (this->op.type) {
		case ref(OperationType_File):
			if (!SocketConnection_SendFile(this->conn,
				&this->op.file.file,
				&this->op.file.offset,
				(size_t) (this->op.file.length - this->op.file.offset)))
			{
				return;
			}

			if (this->op.file.offset == this->op.file.length) {
				this->op.type = ref(OperationType_Idle);

				callback(this->op.file.onDone,
					&this->op.file.file);
			}

			break;

		case ref(OperationType_Buffer):
			while (this->op.buffer.offset < this->op.buffer.s.len) {
				ssize_t len = SocketConnection_Write(this->conn,
					this->op.buffer.s.buf +
					this->op.buffer.offset,

					this->op.buffer.s.len -
					this->op.buffer.offset);

				if (len == -1) {
					return;
				}

				this->op.buffer.offset += len;
			}

			if (this->op.buffer.offset == this->op.buffer.s.len) {
				this->op.type = ref(OperationType_Idle);

				callback(this->op.buffer.onDone,
					&this->op.buffer.s);
			}

			break;

		case ref(OperationType_Idle):
			break;
	}
}

inline def(void, Flush) {
	SocketConnection_Flush(this->conn);
}

inline def(bool, IsIdle) {
	return this->op.type == ref(OperationType_Idle);
}
