#import "StringStream.h"

void StringStream_Init(StringStream *this, String *s) {
	this->str    = s;
	this->offset = -1;
}

size_t StringStream_Read(StringStream *this, void *buf, size_t len) {
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

size_t StringStream_Write(__unused StringStream *this, __unused void *buf, __unused size_t len) {
	return 0;
}

void StringStream_Close(__unused StringStream *this) {

}

bool StringStream_IsEof(StringStream *this) {
	return (size_t) this->offset == this->str->len;
}

StreamInterface StringStreamImpl = {
	.read  = (void *) StringStream_Read,
	.write = (void *) StringStream_Write,
	.close = (void *) StringStream_Close,
	.isEof = (void *) StringStream_IsEof
};
