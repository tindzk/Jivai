#ifndef PATH_H
#define PATH_H

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/syscall.h>

#include "String.h"
#include "Exception.h"

Exception_Export(Path_AccessDeniedException);
Exception_Export(Path_AlreadyExistsException);
Exception_Export(Path_CreationFailedException);
Exception_Export(Path_EmptyPathException);
Exception_Export(Path_InsufficientSpaceException);
Exception_Export(Path_IsDirectoryException);
Exception_Export(Path_NameTooLongException);
Exception_Export(Path_NonExistentPathException);
Exception_Export(Path_NotDirectoryException);
Exception_Export(Path_ResolvingFailedException);
Exception_Export(Path_StatFailedException);
Exception_Export(Path_TruncatingFailedException);

void Path0(ExceptionManager *e);

bool Path_Exists(String path);
String Path_GetCwd(void);
struct stat64 Path_GetStat(String path);
off64_t Path_GetSize(String path);
bool OVERLOAD Path_IsFile(String path);
bool OVERLOAD Path_IsFile(struct stat64 attr);
bool OVERLOAD Path_IsDirectory(String path);
bool OVERLOAD Path_IsDirectory(struct stat64 attr);
void OVERLOAD Path_Truncate(String path, off64_t length);
void OVERLOAD Path_Truncate(String path);
String OVERLOAD Path_GetFilename(String path, bool verify);
String OVERLOAD Path_GetFilename(String path);
String OVERLOAD Path_GetDirectory(String path, bool verify);
String OVERLOAD Path_GetDirectory(String path);
String Path_Resolve(String path);
void OVERLOAD Path_Create(String path, int mode, bool recursive);
void OVERLOAD Path_Create(String path, bool recursive);
void OVERLOAD Path_Create(String path, int mode);
void OVERLOAD Path_Create(String path);

#endif
