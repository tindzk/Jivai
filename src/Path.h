#import <errno.h>
#import <stdlib.h>
#import <attr/xattr.h>
#import <sys/syscall.h>

#import "Time.h"
#import "Stat.h"
#import "String.h"
#import "Exception.h"

#ifndef AT_FDCWD
#define AT_FDCWD -100
#endif

#ifndef AT_SYMLINK_NOFOLLOW
#define AT_SYMLINK_NOFOLLOW 0x100
#endif

Exception_Export(Path_AccessDeniedException);
Exception_Export(Path_AlreadyExistsException);
Exception_Export(Path_AttributeNonExistentException);
Exception_Export(Path_BufferTooSmallException);
Exception_Export(Path_CreationFailedException);
Exception_Export(Path_DeletingFailedException);
Exception_Export(Path_DirectoryNotEmptyException);
Exception_Export(Path_EmptyPathException);
Exception_Export(Path_GettingAttributeFailedException);
Exception_Export(Path_InsufficientSpaceException);
Exception_Export(Path_IsDirectoryException);
Exception_Export(Path_NameTooLongException);
Exception_Export(Path_NonExistentFileException);
Exception_Export(Path_NonExistentPathException);
Exception_Export(Path_NotDirectoryException);
Exception_Export(Path_PermissionDeniedException);
Exception_Export(Path_ReadingLinkFailedException);
Exception_Export(Path_ResolvingFailedException);
Exception_Export(Path_SettingAttributeFailedException);
Exception_Export(Path_SettingTimeFailedException);
Exception_Export(Path_StatFailedException);
Exception_Export(Path_TruncatingFailedException);

void Path0(ExceptionManager *e);

bool OVERLOAD Path_Exists(String path, bool follow);
bool OVERLOAD Path_Exists(String path);
String Path_GetCwd(void);
Stat64 Path_GetStat(String path);
off64_t Path_GetSize(String path);
bool OVERLOAD Path_IsFile(String path);
bool OVERLOAD Path_IsFile(Stat64 attr);
bool OVERLOAD Path_IsDirectory(String path);
bool OVERLOAD Path_IsDirectory(Stat64 attr);
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
void Path_Delete(String path);
void Path_DeleteDirectory(String path);
void Path_ReadLink(String path, String *out);
void Path_Symlink(String path1, String path2);
void Path_SetXattr(String path, String name, String value);
String OVERLOAD Path_GetXattr(String path, String name);
void OVERLOAD Path_GetXattr(String path, String name, String *value);
void OVERLOAD Path_SetTime(String path, time_t timestamp, long nano, bool followSymlink);
void OVERLOAD Path_SetTime(String path, time_t timestamp, bool followSymlink);
void OVERLOAD Path_SetTime(String path, time_t timestamp);
