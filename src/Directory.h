#import "String.h"
#import "Kernel.h"
#import "Exception.h"

#undef self
#define self Directory

#ifndef Directory_BufSize
#define Directory_BufSize 1024
#endif

enum {
	excCannotOpenDirectory = excOffset
};

extern size_t Modules_Directory;

typedef struct {
	unsigned long inode;
	unsigned long offset;
	unsigned short reclen;
	char name[];
} Directory_LinuxEntry;

typedef struct {
	int fd;
	Directory_LinuxEntry *d;
	int nread;
	char buf[Directory_BufSize];
	int bpos;
} Directory;

typedef enum {
	Directory_ItemType_Unknown     =  0,
	Directory_ItemType_FIFO        =  1,
	Directory_ItemType_CharDevice  =  2,
	Directory_ItemType_Directory   =  4,
	Directory_ItemType_BlockDevice =  6,
	Directory_ItemType_Regular     =  8,
	Directory_ItemType_Symlink     = 10,
	Directory_ItemType_Socket      = 12,
	Directory_ItemType_Whiteout    = 14
} Directory_ItemType;

typedef struct {
	long inode;
	Directory_ItemType type;
	String name;
} Directory_Entry;

void Directory0(ExceptionManager *e);

void Directory_Init(Directory *this, String path);
void Directory_Destroy(Directory *this);
bool Directory_Read(Directory *this, Directory_Entry *res);
