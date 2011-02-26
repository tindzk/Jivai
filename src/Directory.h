#import "String.h"
#import "Kernel.h"
#import "Exception.h"

#define self Directory

#ifndef Directory_BufSize
#define Directory_BufSize 1024
#endif

// @exc CannotOpenDirectory
// @exc ReadingFailed

record(ref(LinuxEntry)) {
	unsigned long inode;
	unsigned long offset;
	unsigned short reclen;
	char name[];
};

class {
	ssize_t fd;
	ref(LinuxEntry) *d;
	ssize_t nread;
	char buf[Directory_BufSize];
	int bpos;
};

set(ref(ItemType)) {
	ref(ItemType_Unknown)     =  0,
	ref(ItemType_FIFO)        =  1,
	ref(ItemType_CharDevice)  =  2,
	ref(ItemType_Directory)   =  4,
	ref(ItemType_BlockDevice) =  6,
	ref(ItemType_Regular)     =  8,
	ref(ItemType_Symlink)     = 10,
	ref(ItemType_Socket)      = 12,
	ref(ItemType_Whiteout)    = 14
};

record(ref(Entry)) {
	long inode;
	ref(ItemType) type;
	ProtString name;
};

def(void, Init, ProtString path);
def(void, Destroy);
def(bool, Read, ref(Entry) *res);

#undef self
