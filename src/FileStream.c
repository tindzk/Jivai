#import "FileStream.h"

/* Clang does not support pointers to overloaded C functions. */
size_t FileStream_Write(FileStream *this, void *buf, size_t len) {
	return File_Write(this, buf, len);
}

bool FileStream_IsEof(__unused FileStream *this) {
	return false;
}

StreamInterface FileStream_Methods = {
	.read  = (void *) FileStream_Read,
	.write = (void *) FileStream_Write,
	.close = (void *) FileStream_Close,
	.isEof = (void *) FileStream_IsEof
};
