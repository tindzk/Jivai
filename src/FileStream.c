#include "FileStream.h"

/* Clang does not support pointers to overloaded C functions.  */
static void __write(File *this, void *buf, size_t len) {
	File_Write(this, buf, len);
}

StreamInterface FileStream_Methods = {
	.read  = (void *) FileStream_Read,
	.write = (void *) __write,
	.close = (void *) FileStream_Close
};
