#include "BufferedStream.h"

void BufferedStream_Init(BufferedStream *this, StreamInterface *stream, void *data) {
	this->stream = stream;
	this->data  = data;

	this->eof = false;

	this->inbuf  = HeapString(0);
	this->outbuf = HeapString(0);

	this->inbufThreshold = 0;
}

void BufferedStream_Destroy(BufferedStream *this) {
	String_Destroy(&this->inbuf);
	String_Destroy(&this->outbuf);
}

void BufferedStream_SetInputBuffer(BufferedStream *this, size_t size, size_t threshold) {
	this->inbuf = HeapString(size);
	this->inbufThreshold = threshold;
}

void BufferedStream_SetOutputBuffer(BufferedStream *this, size_t size) {
	this->outbuf = HeapString(size);
}

size_t BufferedStream_Read(BufferedStream *this, void *buf, size_t len) {
	if (len == 0) {
		return 0;
	}

	size_t copied = 0;

	if (this->inbuf.len >= len) {
		Memory_Copy(buf, this->inbuf.buf, len);
		String_Crop(&this->inbuf, len);
		copied = len;
	} else if (this->inbuf.len > 0) {
		Memory_Copy(buf, this->inbuf.buf, this->inbuf.len);
		copied = this->inbuf.len;
		this->inbuf.len = 0;

		if (!this->eof) {
			return copied;
		}

		copied += this->stream->read(this->data,
			buf + copied,
			len - copied
		);

		if (copied < len) {
			this->eof = true;
		}
	} else if (!this->eof) {
		if ((copied = this->stream->read(this->data, buf, len)) < len) {
			this->eof = true;
		}
	}

	if (!this->eof) {
		if (this->inbuf.size - this->inbuf.len > this->inbufThreshold) {
			size_t read = this->stream->read(this->data,
				this->inbuf.buf  + this->inbuf.len,
				this->inbuf.size - this->inbuf.len
			);

			if (read < this->inbuf.size - this->inbuf.len) {
				this->eof = true;
			}

			this->inbuf.len += read;
		}
	}

	return copied;
}

size_t BufferedStream_Write(BufferedStream *this, void *buf, size_t len) {
	if (len == 0) {
		return 0;
	}

	String tmp = StackString(0);

	if (this->outbuf.len + len > this->outbuf.size) {
		/* Jam-pack the buffer first. */
		tmp.buf = buf;
		tmp.len = this->outbuf.size - this->outbuf.len;
		String_Append(&this->outbuf, tmp);

		/* Flush the buffer. */
		this->stream->write(this->data, this->outbuf.buf, this->outbuf.len);
		this->outbuf.len = 0;

		/* Handle the remaining chunk. */
		if (len - tmp.len > 0) {
			BufferedStream_Write(this, buf + tmp.len, len - tmp.len);
		}
	} else {
		tmp.buf = buf;
		tmp.len = len;
		String_Append(&this->outbuf, tmp);
	}

	return len;
}

String BufferedStream_Flush(BufferedStream *this) {
	String res = StackString(0);

	if (this->inbuf.len > 0) {
		res.len = this->inbuf.len;
		res.buf = this->inbuf.buf;
		this->inbuf.len = 0;
	}

	if (this->outbuf.len > 0) {
		this->stream->write(this->data, this->outbuf.buf, this->outbuf.len);
		this->outbuf.len = 0;
	}

	return res;
}

void BufferedStream_Reset(BufferedStream *this) {
	this->eof = false;

	this->inbuf.len  = 0;
	this->outbuf.len = 0;
}

void BufferedStream_Close(BufferedStream *this) {
	BufferedStream_Flush(this);
	this->stream->close(this->data);
}

StreamInterface BufferedStream_Methods = {
	.read  = (void *) BufferedStream_Read,
	.write = (void *) BufferedStream_Write,
	.close = (void *) BufferedStream_Close
};
