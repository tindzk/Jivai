#import "StringStream.h"

#define self StringStream

rsdef(self, New, RdStringInst s) {
	return (self) {
		.str    = s.addr,
		.offset = 0
	};
}

def(size_t, Read, WrBuffer buf) {
	if (buf.size > this->str->len - this->offset) {
		buf.size = this->str->len - this->offset;
	}

	if (buf.size > 0) {
		RdBuffer src = {
			.ptr = this->str->buf + this->offset,
			.len = buf.size
		};

		Buffer_Copy(buf, src);
		this->offset += buf.size;
	}

	return buf.size;
}

def(size_t, Write, __unused RdBuffer buf) {
	assert(false);
	return 0;
}

def(void, Close) { }

def(bool, IsEof) {
	return this->offset >= this->str->len;
}

Impl(Stream) = {
	.read  = ref(Read),
	.write = ref(Write),
	.close = ref(Close),
	.isEof = ref(IsEof)
};
