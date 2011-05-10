#import "String.h"
#import "Stream.h"
#import "Buffer.h"
#import "Exception.h"

#define self StringStream

class {
	RdString *str;
	size_t offset;
};

rsdef(self, New, RdStringInst s);
def(size_t, Read, WrBuffer buf);
def(size_t, Write, RdBuffer buf);
def(void, Close);
def(bool, IsEof);

ExportImpl(Stream);

static alwaysInline Stream String_AsStream(RdStringInst s) {
	StringStream stream = StringStream_New(s);
	return StringStream_AsStream(&stream);
}

#undef self
