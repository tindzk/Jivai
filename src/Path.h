#import <errno.h>
#import <stdlib.h>
#import <attr/xattr.h>
#import <sys/syscall.h>

#import "Time.h"
#import "Stat.h"
#import "String.h"
#import "Exception.h"

#undef self
#define self Path

#ifndef AT_FDCWD
#define AT_FDCWD -100
#endif

#ifndef AT_SYMLINK_NOFOLLOW
#define AT_SYMLINK_NOFOLLOW 0x100
#endif

Exception_Export(AccessDeniedException);
Exception_Export(AlreadyExistsException);
Exception_Export(AttributeNonExistentException);
Exception_Export(BufferTooSmallException);
Exception_Export(CreationFailedException);
Exception_Export(DeletingFailedException);
Exception_Export(DirectoryNotEmptyException);
Exception_Export(EmptyPathException);
Exception_Export(GettingAttributeFailedException);
Exception_Export(InsufficientSpaceException);
Exception_Export(IsDirectoryException);
Exception_Export(NameTooLongException);
Exception_Export(NonExistentFileException);
Exception_Export(NonExistentPathException);
Exception_Export(NotDirectoryException);
Exception_Export(PermissionDeniedException);
Exception_Export(ReadingLinkFailedException);
Exception_Export(ResolvingFailedException);
Exception_Export(SettingAttributeFailedException);
Exception_Export(SettingTimeFailedException);
Exception_Export(StatFailedException);
Exception_Export(TruncatingFailedException);

void Path0(ExceptionManager *e);

overload bool Path_Exists(String path, bool follow);
overload bool Path_Exists(String path);
String Path_GetCwd(void);
Stat64 Path_GetStat(String path);
off64_t Path_GetSize(String path);
overload bool Path_IsFile(String path);
overload bool Path_IsFile(Stat64 attr);
overload bool Path_IsDirectory(String path);
overload bool Path_IsDirectory(Stat64 attr);
overload void Path_Truncate(String path, off64_t length);
overload void Path_Truncate(String path);
overload String Path_GetFilename(String path, bool verify);
overload String Path_GetFilename(String path);
overload String Path_GetDirectory(String path, bool verify);
overload String Path_GetDirectory(String path);
String Path_Resolve(String path);
overload void Path_Create(String path, int mode, bool recursive);
overload void Path_Create(String path, bool recursive);
overload void Path_Create(String path, int mode);
overload void Path_Create(String path);
void Path_Delete(String path);
void Path_DeleteDirectory(String path);
void Path_ReadLink(String path, String *out);
void Path_Symlink(String path1, String path2);
void Path_SetXattr(String path, String name, String value);
overload String Path_GetXattr(String path, String name);
overload void Path_GetXattr(String path, String name, String *value);
overload void Path_SetTime(String path, time_t timestamp, long nano, bool followSymlink);
overload void Path_SetTime(String path, time_t timestamp, bool followSymlink);
overload void Path_SetTime(String path, time_t timestamp);
