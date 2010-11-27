#import "FileStream.h"

def(void, Open, String path, int mode) {
	File_Open(this, path, mode);
}

def(void, Close) {
	File_Close(this);
}

/* Clang does not support pointers to overloaded C functions. */
def(size_t, Read, void *buf, size_t len) {
	return File_Read(File_FromObject(this), buf, len);
}

def(size_t, Write, void *buf, size_t len) {
	return File_Write(File_FromObject(this), buf, len);
}

def(bool, IsEof) {
	return false;
}

StreamInterface Impl(self) = {
	.read  = (void *) ref(Read),
	.write = (void *) ref(Write),
	.close = (void *) ref(Close),
	.isEof = (void *) ref(IsEof)
};
