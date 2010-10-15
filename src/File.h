#import <errno.h>

#import "Kernel.h"
#import "String.h"
#import "Exception.h"

#undef self
#define self File

typedef struct {
	int fd;
	bool readable;
	bool writable;
} File;

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

extern size_t Modules_File;

typedef enum {
	File_SeekType_Set = 0,
	File_SeekType_Cur,
	File_SeekType_End
} File_SeekType;

extern File *File_StdIn;
extern File *File_StdOut;
extern File *File_StdErr;

void File0(ExceptionManager *e);

void File_Open(File *this, String path, int mode);
void File_Close(File *this);
void File_SetXattr(File *this, String name, String value);
overload String File_GetXattr(File *this, String name);
overload void File_GetXattr(File *this, String name, String *value);
overload void File_Truncate(File *this, u64 length);
overload void File_Truncate(File *this);
Stat64 File_GetStat(File *this);
u64 File_GetSize(File *this);
size_t File_Read(File *this, void *buf, size_t len);
overload size_t File_Write(File *this, void *buf, size_t len);
overload size_t File_Write(File *this, String s);
u64 File_Seek(File *this, u64 offset, File_SeekType whence);
u64 File_Tell(File *this);
void File_GetContents(String path, String *res);
