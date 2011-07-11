#import "Kernel.h"
#import "String.h"
#import "Channel.h"
#import "Exception.h"

#define self File

exc(AccessDenied)
exc(AlreadyExists)
exc(AttributeNonExistent)
exc(BufferTooSmall)
exc(CannotOpenFile)
exc(GettingAttributeFailed)
exc(InvalidFileDescriptor)
exc(InvalidParameter)
exc(IsFolder)
exc(NotFound)
exc(NotWritable)
exc(SeekingFailed)
exc(SettingAttributeFailed)
exc(StatFailed)
exc(TruncatingFailed)

class {
	Channel ch;
};

set(ref(SeekType)) {
	ref(SeekType_Set) = 0,
	ref(SeekType_Cur),
	ref(SeekType_End)
};

rsdef(self, new, RdString path, int flags);
def(void, destroy);
def(void, setExtendedAttribute, RdString name, RdString value);
overload def(String, getExtendedAttribute, RdString name);
overload def(void, getExtendedAttribute, RdString name, String *value);
overload def(void, truncate, u64 length);
def(Stat64, getMeta);
def(u64, seek, u64 offset, ref(SeekType) whence);
def(u64, tell);
overload def(size_t, read, void *buf, size_t len);
overload def(void, read, String *res);
overload def(size_t, write, void *buf, size_t len);
overload def(size_t, write, RdString s);
overload def(size_t, write, char c);
overload sdef(void, getContents, RdString path, String *res);
overload sdef(String, getContents, RdString path);

static overload alwaysInline def(void, truncate) {
	call(truncate, 0);
}

static overload alwaysInline def(u64, getSize) {
	return call(getMeta).size;
}

#undef self
