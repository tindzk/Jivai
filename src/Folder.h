#import "Path.h"
#import "String.h"
#import "Channel.h"
#import "Exception.h"

#define self Folder

exc(CannotOpenFolder)
exc(ReadingFailed)
exc(SeekingFailed)

#ifndef Folder_BufSize
#define Folder_BufSize 1024
#endif

record(ref(LinuxEntry)) {
	unsigned long inode;
	unsigned long offset;
	unsigned short reclen;
	char name[];
};

class {
	Channel ch;
	ref(LinuxEntry) *d;
	ssize_t nread;
	char buf[Folder_BufSize];
	int bpos;
};

set(ref(ItemType)) {
	ref(ItemType_Unknown)     =  0,
	ref(ItemType_FIFO)        =  1,
	ref(ItemType_CharDevice)  =  2,
	ref(ItemType_Folder)      =  4,
	ref(ItemType_BlockDevice) =  6,
	ref(ItemType_Regular)     =  8,
	ref(ItemType_Symlink)     = 10,
	ref(ItemType_Socket)      = 12,
	ref(ItemType_Whiteout)    = 14
};

record(ref(Entry)) {
	long inode;
	ref(ItemType) type;
	RdString name;
};

rsdef(self, new, RdString path);
def(void, destroy);
def(void, reset);
def(bool, read, ref(Entry) *res);

#undef self
