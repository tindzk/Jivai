#import "StringStream.h"

#define self StringStream

overload rsdef(self, New, StringInst s) {
	return (self) {
		.str  = String_ToCarrier(*s.addr),
		.orig = s.addr
	};
}

overload rsdef(self, New, OmniString s) {
	return (self) {
		.str = String_ToCarrier(s)
	};
}

def(size_t, Read, WrBuffer buf) {
	if (buf.size > this->str.len - this->offset) {
		buf.size = this->str.len - this->offset;
	}

	if (buf.size > 0) {
		RdBuffer src = {
			.ptr = this->str.buf + this->offset,
			.len = buf.size
		};

		Buffer_Copy(buf, src);
		this->offset += buf.size;
	}

	return buf.size;
}

def(size_t, Write, RdBuffer buf) {
	assert(!this->str.omni);

	String_Append((String *) &this->str, (RdString) {
		.buf = buf.ptr,
		.len = buf.len
	});

	this->orig->buf = this->str.buf;
	this->orig->len = this->str.len;

	return buf.len;
}

def(void, Close) { }

def(bool, IsEof) {
	return this->offset >= this->str.len;
}

Impl(Stream) = {
	.read  = ref(Read),
	.write = ref(Write),
	.close = ref(Close),
	.isEof = ref(IsEof)
};
