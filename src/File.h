#import <errno.h>
#import <stdlib.h>
#import <attr/xattr.h>
#import <sys/syscall.h>

#import "Stat.h"
#import "String.h"
#import "Exception.h"

#undef self
#define self File

Exception_Export(AccessDeniedException);
Exception_Export(AlreadyExistsException);
Exception_Export(AttributeNonExistentException);
Exception_Export(BufferTooSmallException);
Exception_Export(CannotOpenFileException);
Exception_Export(GettingAttributeFailedException);
Exception_Export(InvalidFileDescriptorException);
Exception_Export(InvalidParameterException);
Exception_Export(IsDirectoryException);
Exception_Export(NotFoundException);
Exception_Export(NotReadableException);
Exception_Export(NotWritableException);
Exception_Export(ReadingFailedException);
Exception_Export(ReadingInterruptedException);
Exception_Export(SeekingFailedException);
Exception_Export(SettingAttributeFailedException);
Exception_Export(StatFailedException);
Exception_Export(TruncatingFailedException);
Exception_Export(WritingFailedException);
Exception_Export(WritingInterruptedException);

typedef struct {
	int fd;
	bool readable;
	bool writable;
} File;

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
String OVERLOAD File_GetXattr(File *this, String name);
void OVERLOAD File_GetXattr(File *this, String name, String *value);
void OVERLOAD File_Truncate(File *this, off64_t length);
void OVERLOAD File_Truncate(File *this);
Stat64 File_GetStat(File *this);
off64_t File_GetSize(File *this);
size_t File_Read(File *this, void *buf, size_t len);
size_t OVERLOAD File_Write(File *this, void *buf, size_t len);
size_t OVERLOAD File_Write(File *this, String s);
off64_t File_Seek(File *this, off64_t offset, File_SeekType whence);
off64_t File_Tell(File *this);
