#import "String.h"
#import "Stream.h"
#import "Buffer.h"
#import "Exception.h"

#define self StringStream

class {
	CarrierString str;
	String *orig;
	size_t offset;
};

overload rsdef(self, New, StringInst s);
overload rsdef(self, New, OmniString s);
def(size_t, Read, WrBuffer buf);
def(size_t, Write, RdBuffer buf);
def(void, Close);
def(bool, IsEof);

ExportImpl(Stream);

static overload alwaysInline Stream String_AsStream(StringInst s) {
	StringStream stream = StringStream_New(s);
	return StringStream_AsStream(&stream);
}

static overload alwaysInline Stream String_AsStream(OmniString s) {
	StringStream stream = StringStream_New(s);
	return StringStream_AsStream(&stream);
}

#undef self
