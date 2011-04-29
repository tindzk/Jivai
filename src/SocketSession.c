#import "SocketSession.h"

#define self SocketSession

rsdef(self, New, SocketConnection *conn) {
	return (self) {
		.conn    = conn,
		.op.type = ref(OperationType_Idle)
	};
}

def(void, Write, RdString s, ref(OnDone) onDone) {
	assert(this->op.type == ref(OperationType_Idle));

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
	assert(this->op.type == ref(OperationType_Idle));

	this->op.type = ref(OperationType_File);

	this->op.file.offset = 0;
	this->op.file.file   = file;
	this->op.file.length = length;
	this->op.file.onDone = onDone;

	call(Continue);
}

def(void, Continue) {
	assert(this->op.type != ref(OperationType_Idle));

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
