#import "StringStream.h"

#define self StringStream

def(void, Init, String *s) {
	this->str    = s;
	this->offset = 0;
}

def(size_t, Read, void *buf, size_t len) {
	size_t bytes = len;

	if (bytes > this->str->len - this->offset) {
		bytes = this->str->len - this->offset;
	}

	if (bytes > 0) {
		Memory_Copy(buf, this->str->buf + this->offset, bytes);
		this->offset += bytes;
	}

	return bytes;
}

def(size_t, Write, __unused void *buf, __unused size_t len) {
	return 0;
}

def(void, Close) {

}

def(bool, IsEof) {
	return this->offset >= this->str->len;
}

Impl(Stream) = {
	share(Read,  read),
	share(Write, write),
	share(Close, close),
	share(IsEof, isEof)
};
