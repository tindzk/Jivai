#import "BufferedStream.h"

#define self BufferedStream

rsdef(self, New, Stream stream) {
	return (self) {
		.stream = stream,
		.eof    = false,
		.inbuf  = String_New(0),
		.outbuf = String_New(0),
		.inbufThreshold = 0
	};
}

def(void, Destroy) {
	String_Destroy(&this->inbuf);
	String_Destroy(&this->outbuf);
}

def(void, SetInputBuffer, size_t size, size_t threshold) {
	String_Align(&this->inbuf, size);
	this->inbufThreshold = threshold;
}

def(void, SetOutputBuffer, size_t size) {
	String_Align(&this->outbuf, size);
}

def(bool, IsEof) {
	return this->inbuf.len == 0
		&& this->eof;
}

def(size_t, Read, void *buf, size_t len) {
	if (len == 0) {
		return 0;
	}

	size_t size = String_GetSize(this->inbuf);

	if (this->inbuf.len == 0 && !this->eof) {
		this->inbuf.len = delegate(this->stream, read,
			this->inbuf.buf, size);

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
		if (size - this->inbuf.len > this->inbufThreshold) {
			size_t read = delegate(this->stream, read,
				this->inbuf.buf + this->inbuf.len,
				size - this->inbuf.len);

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

	size_t size = String_GetSize(this->outbuf);

	if (this->outbuf.len + len > size) {
		size_t bufLength = size - this->outbuf.len;

		/* Jam-pack the buffer first. */
		String_Append(&this->outbuf, (ProtString) {
			.buf = buf,
			.len = bufLength
		});

		/* Flush the buffer. */
		delegate(this->stream, write,
			this->outbuf.buf,
			this->outbuf.len);

		this->outbuf.len = 0;

		/* Handle the remaining chunk. */
		if (len - bufLength > 0) {
			BufferedStream_Write(this,
				buf + bufLength,
				len - bufLength);
		}
	} else {
		String_Append(&this->outbuf, (ProtString) {
			.buf = buf,
			.len = len
		});
	}

	return len;
}

def(ProtString, Flush) {
	ProtString res = $("");

	if (this->inbuf.len > 0) {
		res = this->inbuf.prot;
		this->inbuf.len = 0;
	}

	if (this->outbuf.len > 0) {
		delegate(this->stream, write,
			this->outbuf.buf,
			this->outbuf.len);

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
	delegate(this->stream, close);
}

Impl(Stream) = {
	.read  = (void *) ref(Read),
	.write = (void *) ref(Write),
	.close = (void *) ref(Close),
	.isEof = (void *) ref(IsEof)
};
