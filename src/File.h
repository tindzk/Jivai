#import <errno.h>

#import "Kernel.h"
#import "String.h"
#import "Exception.h"

#define self File

class {
	ssize_t fd;
	bool readable;
	bool writable;
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
// @exc NotReadable
// @exc NotWritable
// @exc ReadingFailed
// @exc ReadingInterrupted
// @exc SeekingFailed
// @exc SettingAttributeFailed
// @exc StatFailed
// @exc TruncatingFailed
// @exc WritingFailed
// @exc WritingInterrupted

set(ref(SeekType)) {
	ref(SeekType_Set) = 0,
	ref(SeekType_Cur),
	ref(SeekType_End)
};

extern self* ref(StdIn);
extern self* ref(StdOut);
extern self* ref(StdErr);

def(void, Open, ProtString path, int mode);
def(void, Close);
def(void, SetXattr, ProtString name, ProtString value);
overload def(String, GetXattr, ProtString name);
overload def(void, GetXattr, ProtString name, String *value);
overload def(void, Truncate, u64 length);
overload def(void, Truncate);
def(Stat64, GetStat);
def(u64, GetSize);
overload def(size_t, Read, void *buf, size_t len);
overload def(void, Read, String *res);
overload def(size_t, Write, void *buf, size_t len);
overload def(size_t, Write, ProtString s);
overload def(size_t, Write, char c);
def(u64, Seek, u64 offset, ref(SeekType) whence);
def(u64, Tell);
sdef(void, GetContents, ProtString path, String *res);

#define File_Read(obj, ...) \
	File_Read(File_FromObject(obj), ## __VA_ARGS__)

#define File_Write(obj, ...) \
	File_Write(File_FromObject(obj), ## __VA_ARGS__)

#undef self
