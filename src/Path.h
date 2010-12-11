#import <errno.h>

#import "Time.h"
#import "Kernel.h"
#import "String.h"
#import "Exception.h"

#define self Path

#ifndef AT_FDCWD
#define AT_FDCWD -100
#endif

#ifndef AT_SYMLINK_NOFOLLOW
#define AT_SYMLINK_NOFOLLOW 0x100
#endif

// @exc AccessDenied
// @exc AlreadyExists
// @exc AttributeNonExistent
// @exc BufferTooSmall
// @exc CreationFailed
// @exc DeletingFailed
// @exc DirectoryNotEmpty
// @exc EmptyPath
// @exc GettingAttributeFailed
// @exc InsufficientSpace
// @exc IsDir
// @exc NameTooLong
// @exc NonExistentFile
// @exc NonExistentPath
// @exc NotDirectory
// @exc PermissionDenied
// @exc ReadingLinkFailed
// @exc ResolvingFailed
// @exc SettingAttributeFailed
// @exc SettingTimeFailed
// @exc StatFailed
// @exc TruncatingFailed

overload sdef(bool, Exists, String path, bool follow);
overload sdef(bool, Exists, String path);
sdef(String, GetCwd);
sdef(Stat64, GetStat, String path);
sdef(u64, GetSize, String path);
overload sdef(bool, IsFile, String path);
overload sdef(bool, IsFile, Stat64 attr);
overload sdef(bool, IsDirectory, String path);
overload sdef(bool, IsDirectory, Stat64 attr);
overload sdef(void, Truncate, String path, u64 length);
overload sdef(void, Truncate, String path);
overload sdef(String, GetFilename, String path, bool verify);
overload sdef(String, GetFilename, String path);
overload sdef(String, GetDirectory, String path, bool verify);
overload sdef(String, GetDirectory, String path);
sdef(String, Resolve, String path);
overload sdef(void, Create, String path, int mode, bool recursive);
overload sdef(void, Create, String path, bool recursive);
overload sdef(void, Create, String path, int mode);
overload sdef(void, Create, String path);
sdef(void, Delete, String path);
sdef(void, DeleteDirectory, String path);
sdef(void, ReadLink, String path, String *out);
sdef(void, Symlink, String path1, String path2);
sdef(void, SetXattr, String path, String name, String value);
overload sdef(String, GetXattr, String path, String name);
overload sdef(void, GetXattr, String path, String name, String *value);
overload sdef(void, SetTime, String path, time_t timestamp, long nano, bool followSymlink);
overload sdef(void, SetTime, String path, time_t timestamp, bool followSymlink);
overload sdef(void, SetTime, String path, time_t timestamp);

#undef self
