#import "FileStream.h"

#define self FileStream

def(void, Close) {
	File_Destroy(this);
}

def(size_t, Read, WrBuffer buf) {
	return File_Read(this, buf.ptr, buf.size);
}

def(size_t, Write, RdBuffer buf) {
	return File_Write(this, buf.ptr, buf.len);
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
