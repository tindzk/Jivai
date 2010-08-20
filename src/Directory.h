#import <dirent.h> /* Defines DT_* constants. */
#import <sys/syscall.h>

#import "Stat.h"
#import "String.h"
#import "Exception.h"

#ifndef Directory_BufSize
#define Directory_BufSize 1024
#endif

Exception_Export(Directory_CannotOpenDirectoryException);
Exception_Export(Directory_ReadingFailedException);

typedef struct {
	int fd;
	struct linux_dirent *d;
	int nread;
	char buf[Directory_BufSize];
	int bpos;
} Directory;

typedef struct {
	unsigned long inode;
	unsigned long offset;
	unsigned short reclen;
	char name[];
} Directory_LinuxEntry;

typedef struct {
	long inode;
	char type;
	String name;
} Directory_Entry;

void Directory0(ExceptionManager *e);

void Directory_Init(Directory *this, String path);
void Directory_Destroy(Directory *this);
bool Directory_Read(Directory *this, Directory_Entry *res);
