#import "String.h"
#import "StreamInterface.h"

#define self StringStream

class {
	ProtString *str;
	size_t offset;
};

def(void, Init, ProtStringInstance s);
def(size_t, Read, void *buf, size_t len);
def(size_t, Write, __unused void *buf, __unused size_t len);
def(void, Close);
def(bool, IsEof);

ExportImpl(Stream);

static alwaysInline Stream String_AsStream(ProtStringInstance s) {
	StringStream stream;
	StringStream_Init(&stream, s);

	return StringStream_AsStream(&stream);
}

#undef self
