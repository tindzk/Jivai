#import "FileStream.h"

#define self FileStream

def(void, Close) {
	File_Destroy(this);
}

/* Clang does not support pointers to overloaded C functions. */
def(size_t, Read, void *buf, size_t len) {
	return File_Read(this, buf, len);
}

def(size_t, Write, void *buf, size_t len) {
	return File_Write(this, buf, len);
}

def(bool, IsEof) {
	return false;
}

Impl(Stream) = {
	.read  = ref(Read),
	.write = ref(Write),
	.close = ref(Close),
	.isEof = ref(IsEof)
};
