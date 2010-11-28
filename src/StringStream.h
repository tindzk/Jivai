#import "String.h"
#import "StreamInterface.h"

#undef self
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

StreamInterface Impl(self);
