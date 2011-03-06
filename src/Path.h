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

overload sdef(bool, Exists, ProtString path, bool follow);
overload sdef(bool, Exists, ProtString path);
sdef(String, GetCwd);
sdef(Stat64, GetStat, ProtString path);
sdef(u64, GetSize, ProtString path);
overload sdef(bool, IsFile, ProtString path);
overload sdef(bool, IsFile, Stat64 attr);
overload sdef(bool, IsDirectory, ProtString path);
overload sdef(bool, IsDirectory, Stat64 attr);
overload sdef(void, Truncate, ProtString path, u64 length);
overload sdef(void, Truncate, ProtString path);
sdef(ProtString, GetExtension, ProtString path);
overload sdef(ProtString, GetFilename, ProtString path, bool verify);
overload sdef(ProtString, GetFilename, ProtString path);
overload sdef(ProtString, GetDirectory, ProtString path, bool verify);
overload sdef(ProtString, GetDirectory, ProtString path);
sdef(String, Resolve, ProtString path);
overload sdef(void, Create, ProtString path, int mode, bool recursive);
overload sdef(void, Create, ProtString path, bool recursive);
overload sdef(void, Create, ProtString path, int mode);
overload sdef(void, Create, ProtString path);
sdef(void, Delete, ProtString path);
sdef(void, DeleteDirectory, ProtString path);
sdef(void, ReadLink, ProtString path, String *out);
sdef(void, Symlink, ProtString path1, ProtString path2);
sdef(void, SetXattr, ProtString path, ProtString name, ProtString value);
overload sdef(String, GetXattr, ProtString path, ProtString name);
overload sdef(void, GetXattr, ProtString path, ProtString name, String *value);
overload sdef(void, SetTime, ProtString path, time_t timestamp, long nano, bool followSymlink);
overload sdef(void, SetTime, ProtString path, time_t timestamp, bool followSymlink);
overload sdef(void, SetTime, ProtString path, time_t timestamp);

#undef self
