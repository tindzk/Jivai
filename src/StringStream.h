#import "String.h"
#import "Stream.h"
#import "Exception.h"

#define self StringStream

class {
	RdString *str;
	size_t offset;
};

rsdef(self, New, RdStringInst s);
def(size_t, Read, void *buf, size_t len);
def(size_t, Write, __unused void *buf, __unused size_t len);
def(void, Close);
def(bool, IsEof);

ExportImpl(Stream);

static alwaysInline Stream String_AsStream(RdStringInst s) {
	StringStream stream = StringStream_New(s);
	return StringStream_AsStream(&stream);
}

#undef self
