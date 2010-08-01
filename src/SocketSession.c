#include "SocketSession.h"

Exception_Define(SocketSession_NotIdleException);

static ExceptionManager *exc;

void SocketSession0(ExceptionManager *e) {
	exc = e;
}

void SocketSession_Init(SocketSession *this, SocketConnection *conn, void *context) {
	this->conn    = conn;
	this->context = context;

	this->operation.type = SocketSession_OperationType_Idle;
}

void SocketSession_Write(SocketSession *this, String s, SocketSession_OnDone onDone) {
	if (!SocketSession_IsIdle(this)) {
		throw(exc, &SocketSession_NotIdleException);
	}

	if (s.len == 0) {
		onDone(this->context, &s);
		return;
	}

	this->operation.type = SocketSession_OperationType_Buffer;

	this->operation.u.buffer.buffer = s;
	this->operation.u.buffer.offset = 0;
	this->operation.u.buffer.onDone = onDone;

	SocketSession_Continue(this);
}

void SocketSession_SendFile(SocketSession *this, File file, size_t length, SocketSession_OnDone onDone) {
	if (!SocketSession_IsIdle(this)) {
		throw(exc, &SocketSession_NotIdleException);
	}

	this->operation.type = SocketSession_OperationType_File;

	this->operation.u.file.offset = 0;
	this->operation.u.file.file   = file;
	this->operation.u.file.length = length;
	this->operation.u.file.onDone = onDone;

	SocketSession_Continue(this);
}

void SocketSession_Continue(SocketSession *this) {
	if (this->operation.type == SocketSession_OperationType_File) {
		if (!SocketConnection_SendFile(this->conn,
			&this->operation.u.file.file,
			&this->operation.u.file.offset,
			 this->operation.u.file.length
		   - this->operation.u.file.offset))
		{
			return;
		}

		if (this->operation.u.file.offset == this->operation.u.file.length) {
			this->operation.type = SocketSession_OperationType_Idle;

			this->operation.u.file.onDone(this->context,
				&this->operation.u.file.file);
		}
	} else if (this->operation.type == SocketSession_OperationType_Buffer) {
		while (this->operation.u.buffer.offset <
				this->operation.u.buffer.buffer.len)
		{
			ssize_t len = SocketConnection_Write(this->conn,
				this->operation.u.buffer.buffer.buf +
				this->operation.u.buffer.offset,

				this->operation.u.buffer.buffer.len -
				this->operation.u.buffer.offset);

			if (len == -1) {
				return;
			}

			this->operation.u.buffer.offset += len;
		}

		if (this->operation.u.buffer.offset ==
			this->operation.u.buffer.buffer.len)
		{
			this->operation.type = SocketSession_OperationType_Idle;

			this->operation.u.buffer.onDone(this->context,
				&this->operation.u.buffer.buffer);
		}
	}
}

inline bool SocketSession_IsIdle(SocketSession *this) {
	return this->operation.type == SocketSession_OperationType_Idle;
}
