#import <errno.h>

#import "Kernel.h"
#import "String.h"
#import "Exception.h"

#undef self
#define self File

class(self) {
	ssize_t fd;
	bool readable;
	bool writable;
};

ExtendClass(self);

enum {
	excCannotOpenFile = excOffset,
	excNotFound,
	excNotReadable,
	excNotWritable,
	excReadingInterrupted,
	excSeekingFailed,
	excWritingFailed,
	excWritingInterrupted
};

set(ref(SeekType)) {
	ref(SeekType_Set) = 0,
	ref(SeekType_Cur),
	ref(SeekType_End)
};

extern self* ref(StdIn);
extern self* ref(StdOut);
extern self* ref(StdErr);

def(void, Open, String path, int mode);
def(void, Close);
def(void, SetXattr, String name, String value);
overload def(String, GetXattr, String name);
overload def(void, GetXattr, String name, String *value);
overload def(void, Truncate, u64 length);
overload def(void, Truncate);
def(Stat64, GetStat);
def(u64, GetSize);
overload def(size_t, Read, void *buf, size_t len);
overload def(void, Read, String *res);
overload def(size_t, Write, void *buf, size_t len);
overload def(size_t, Write, String s);
def(u64, Seek, u64 offset, ref(SeekType) whence);
def(u64, Tell);
void ref(GetContents)(String path, String *res);
