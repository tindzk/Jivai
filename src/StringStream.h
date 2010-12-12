#import "String.h"
#import "StreamInterface.h"

#define self StringStream

class {
	String *str;
	size_t offset;
};

def(void, Init, String *s);
def(size_t, Read, void *buf, size_t len);
def(size_t, Write, __unused void *buf, __unused size_t len);
def(void, Close);
def(bool, IsEof);

ExportImpl(Stream);

static inline Stream String_AsStream(String *s) {
	StringStream stream;
	StringStream_Init(&stream, s);

	return StringStream_AsStream(&stream);
}

#undef self
