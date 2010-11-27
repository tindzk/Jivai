#import "BufferedStream.h"

def(void, Init, StreamInterface *stream, void *data) {
	this->stream = stream;
	this->data  = data;

	this->eof = false;

	this->inbuf  = HeapString(0);
	this->outbuf = HeapString(0);

	this->inbufThreshold = 0;
}

def(void, Destroy) {
	String_Destroy(&this->inbuf);
	String_Destroy(&this->outbuf);
}

def(void, SetInputBuffer, size_t size, size_t threshold) {
	if (this->inbuf.size == 0) {
		this->inbuf = HeapString(size);
	} else {
		String_Align(&this->inbuf, size);
	}

	this->inbufThreshold = threshold;
}

def(void, SetOutputBuffer, size_t size) {
	if (this->outbuf.size == 0) {
		this->outbuf = HeapString(size);
	} else {
		String_Align(&this->outbuf, size);
	}
}

def(bool, IsEof) {
	return this->inbuf.len == 0 && this->eof;
}

def(size_t, Read, void *buf, size_t len) {
	if (len == 0) {
		return 0;
	}

	if (this->inbuf.len == 0 && !this->eof) {
		this->inbuf.len = this->stream->read(this->data,
			this->inbuf.buf,
			this->inbuf.size);

		if (this->inbuf.len == 0) {
			this->eof = true;
		}
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
	}

	if (!this->eof) {
		if (this->inbuf.size - this->inbuf.len > this->inbufThreshold) {
			size_t read = this->stream->read(this->data,
				this->inbuf.buf  + this->inbuf.len,
				this->inbuf.size - this->inbuf.len);

			if (read == 0) {
				this->eof = true;
			}

			this->inbuf.len += read;
		}
	}

	return copied;
}

def(size_t, Write, void *buf, size_t len) {
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
			BufferedStream_Write(this,
				buf + tmp.len,
				len - tmp.len);
		}
	} else {
		tmp.buf = buf;
		tmp.len = len;
		String_Append(&this->outbuf, tmp);
	}

	return len;
}

def(String, Flush) {
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

def(void, Reset) {
	this->eof = false;

	this->inbuf.len  = 0;
	this->outbuf.len = 0;
}

def(void, Close) {
	BufferedStream_Flush(this);
	this->stream->close(this->data);
}

StreamInterface Impl(self) = {
	.read  = (void *) ref(Read),
	.write = (void *) ref(Write),
	.close = (void *) ref(Close),
	.isEof = (void *) ref(IsEof)
};
