#import "Kernel.h"
#import "String.h"
#import "Channel.h"
#import "Exception.h"

#define self File

class {
	Channel ch;
};

// @exc AccessDenied
// @exc AlreadyExists
// @exc AttributeNonExistent
// @exc BufferTooSmall
// @exc CannotOpenFile
// @exc GettingAttributeFailed
// @exc InvalidFileDescriptor
// @exc InvalidParameter
// @exc IsDirectory
// @exc NotFound
// @exc NotWritable
// @exc SeekingFailed
// @exc SettingAttributeFailed
// @exc StatFailed
// @exc TruncatingFailed

set(ref(SeekType)) {
	ref(SeekType_Set) = 0,
	ref(SeekType_Cur),
	ref(SeekType_End)
};

rsdef(self, New, RdString path, int flags);
def(void, Destroy);
def(void, SetXattr, RdString name, RdString value);
overload def(String, GetXattr, RdString name);
overload def(void, GetXattr, RdString name, String *value);
overload def(void, Truncate, u64 length);
overload def(void, Truncate);
def(Stat64, GetStat);
def(u64, GetSize);
def(u64, Seek, u64 offset, ref(SeekType) whence);
def(u64, Tell);
overload def(size_t, Read, void *buf, size_t len);
overload def(void, Read, String *res);
overload def(size_t, Write, char *buf, size_t len);
overload def(size_t, Write, RdString s);
overload def(size_t, Write, char c);
sdef(void, GetContents, RdString path, String *res);

#undef self
