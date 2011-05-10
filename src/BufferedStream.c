#import "BufferedStream.h"

#define self BufferedStream

rsdef(self, New, Stream stream) {
	return (self) {
		.stream = stream
	};
}

def(void, Destroy) {
	if (Buffer_IsValid(this->in.rd)) {
		Buffer_Destroy(&this->in);
	}

	if (Buffer_IsValid(this->out.rd)) {
		Buffer_Destroy(&this->out);
	}
}

def(void, SetInputBuffer, size_t size, size_t threshold) {
	if (this->in.ptr == NULL) {
		this->in = Buffer_New(size);
	} else {
		Buffer_Align(&this->in, size);
	}

	this->inThreshold = threshold;
}

def(void, SetOutputBuffer, size_t size) {
	if (this->out.ptr == NULL) {
		this->out = Buffer_New(size);
	} else {
		Buffer_Align(&this->out, size);
	}
}

def(bool, IsEof) {
	return this->in.len == 0
		&& this->eof;
}

def(size_t, Read, WrBuffer buf) {
	assert(buf.size > 0);

	if (this->in.len == 0) {
		if (!this->eof) {
			this->in.len = delegate(this->stream, read,
				Buffer_AsWrBuffer(&this->in));
		}

		if (this->in.len == 0) {
			this->eof = true;
			return 0;
		}
	}

	size_t res = 0;

	if (buf.size >= this->in.len) {
		Buffer_Copy(buf, this->in.rd);
		this->in.len = 0;

		res = this->in.len;
	} else {
		/* We have more data than `buf' can take. */
		Buffer_Copy(buf, (RdBuffer) {
			.ptr = this->in.ptr,
			.len = buf.size
		});

		Buffer_Move(Buffer_AsWrBuffer(&this->in), (RdBuffer) {
			.ptr = this->in.ptr + buf.size,
			.len = this->in.len - buf.size
		});

		this->in.len -= buf.size;

		res = buf.size;
	}

	if (!this->eof) {
		size_t size = Buffer_GetSize(&this->in);
		size_t nonOccupied = size - this->in.len;

		if (nonOccupied >= this->inThreshold) {
			size_t read = delegate(this->stream, read, (WrBuffer) {
				.ptr  = this->in.ptr + this->in.len,
				.size = nonOccupied
			});

			if (read == 0) {
				this->eof = true;
			}

			this->in.len += read;
		}
	}

	return res;
}

def(size_t, Write, RdBuffer buf) {
	if (buf.len == 0) {
		return 0;
	}

	size_t size = Buffer_GetSize(&this->out);

	if (size >= this->out.len + buf.len) {
		/* this->out is large enough for the whole buffer. */
		Buffer_Copy((WrBuffer) {
			.ptr  = this->out.ptr + this->out.len,
			.size = size          - this->out.len
		}, buf);

		this->out.len += buf.len;
	} else {
		size_t bufLength = size - this->out.len;

		/* Jam-pack the buffer first. */
		Buffer_Copy(
			(WrBuffer) {
				.ptr  = this->out.ptr + this->out.len,
				.size = size          - this->out.len
			},

			(RdBuffer) {
				.ptr = buf.ptr,
				.len = bufLength
			}
		);

		this->out.len += buf.len;

		/* Flush the buffer. */
		size_t written = delegate(this->stream, write, this->out.rd);
		assert(written == this->out.len);
		this->out.len = 0;

		/* Handle the remaining chunk(s). */
		assert(buf.len - bufLength != 0);
		call(Write, (RdBuffer) {
			.ptr = buf.ptr + bufLength,
			.len = buf.len - bufLength
		});
	}

	return buf.len;
}

def(RdBuffer, Flush) {
	RdBuffer res = { .ptr = NULL, .len = 0 };

	if (this->in.len > 0) {
		res = this->in.rd;
		this->in.len = 0;
	}

	if (this->out.len > 0) {
		size_t written = delegate(this->stream, write, this->out.rd);
		assert(written == this->out.len);
		this->out.len = 0;
	}

	return res;
}

def(void, Reset) {
	this->eof = false;

	this->in.len  = 0;
	this->out.len = 0;
}

def(void, Close) {
	BufferedStream_Flush(this);
	delegate(this->stream, close);
}

Impl(Stream) = {
	.read  = ref(Read),
	.write = ref(Write),
	.close = ref(Close),
	.isEof = ref(IsEof)
};
