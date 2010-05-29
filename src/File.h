#ifndef FILE_H
#define FILE_H

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "Logger.h"
#include "Exception.h"

Exception_Export(File_AccessDeniedException);
Exception_Export(File_AlreadyExistsException);
Exception_Export(File_CannotOpenFileException);
Exception_Export(File_InvalidFileDescriptorException);
Exception_Export(File_InvalidParameterException);
Exception_Export(File_IsDirectoryException);
Exception_Export(File_NotFoundException);
Exception_Export(File_NotReadableException);
Exception_Export(File_NotWritableException);
Exception_Export(File_ReadingFailedException);
Exception_Export(File_ReadingInterruptedException);
Exception_Export(File_SeekingFailedException);
Exception_Export(File_StatFailedException);
Exception_Export(File_WritingFailedException);
Exception_Export(File_WritingInterruptedException);

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
struct stat64 File_GetStat(File *this);
off64_t File_GetSize(File *this);
size_t File_Read(File *this, void *buf, size_t len);
size_t File_Write(File *this, void *buf, size_t len);
off64_t File_Seek(File *this, off64_t offset, File_SeekType whence);
off64_t File_Tell(File *this);

#define File_WriteString(out, s) \
	File_Write(out, (s).buf, (s).len)

#endif
