#import "Time.h"
#import "Kernel.h"
#import "String.h"
#import "Exception.h"

#define self Path

exc(AccessDenied)
exc(AlreadyExists)
exc(AttributeNonExistent)
exc(BufferTooSmall)
exc(CreationFailed)
exc(DeletingFailed)
exc(DirectoryNotEmpty)
exc(GettingAttributeFailed)
exc(InsufficientSpace)
exc(IsDir)
exc(NameTooLong)
exc(NonExistentFile)
exc(NonExistentPath)
exc(NotDirectory)
exc(PermissionDenied)
exc(ReadingLinkFailed)
exc(ResolvingFailed)
exc(SettingAttributeFailed)
exc(SettingTimeFailed)
exc(StatFailed)
exc(TruncatingFailed)

#ifndef AT_FDCWD
#define AT_FDCWD -100
#endif

#ifndef AT_SYMLINK_NOFOLLOW
#define AT_SYMLINK_NOFOLLOW 0x100
#endif

overload sdef(bool, Exists, RdString path, bool follow);
sdef(String, GetCwd);
sdef(Stat, getLinkStat, RdString path);
sdef(Stat64, GetStat, RdString path);
sdef(u64, GetSize, RdString path);
overload sdef(bool, isDirectory, RdString path);
overload sdef(void, Truncate, RdString path, u64 length);
sdef(RdString, GetExtension, RdString path);
overload sdef(RdString, GetFilename, RdString path, bool verify);
overload sdef(RdString, GetDirectory, RdString path, bool verify);
sdef(RdString, getDirectoryName, RdString path);
sdef(String, expandDirectory, RdString path);
sdef(String, expandFile, RdString path);
overload sdef(String, expand, RdString path, bool follow);
overload sdef(void, Create, RdString path, int mode, bool recursive);
sdef(void, Delete, RdString path);
sdef(void, DeleteDirectory, RdString path);
overload sdef(void, followLink, RdString path, String *out);
overload sdef(String, followLink, RdString path);
sdef(void, Symlink, RdString path1, RdString path2);
sdef(void, SetXattr, RdString path, RdString name, RdString value);
overload sdef(String, GetXattr, RdString path, RdString name);
overload sdef(void, GetXattr, RdString path, RdString name, String *value);
overload sdef(void, SetTime, RdString path, time_t timestamp, long nano, bool followSymlink);

static inline overload sdef(bool, Exists, RdString path) {
	return scall(Exists, path, false);
}

static inline overload sdef(bool, IsFile, RdString path) {
	return scall(GetStat, path).mode & FileMode_Regular;
}

static inline overload sdef(bool, IsFile, Stat64 attr) {
	return attr.mode & FileMode_Regular;
}

static inline overload sdef(bool, isDirectory, Stat64 attr) {
	return attr.mode & FileMode_Directory;
}

static inline overload sdef(bool, isLink, RdString path) {
	return scall(getLinkStat, path).mode & FileMode_Link;
}

static inline overload sdef(bool, isLink, Stat64 attr) {
	return attr.mode & FileMode_Link;
}

static inline overload sdef(void, Truncate, RdString path) {
	scall(Truncate, path, 0);
}

static inline overload sdef(RdString, GetFilename, RdString path) {
	return scall(GetFilename, path, true);
}

static inline overload sdef(RdString, GetDirectory, RdString path) {
	return scall(GetDirectory, path, true);
}

static inline overload sdef(void, Create, RdString path, bool recursive) {
	scall(Create, path,
		Permission_OwnerRead    |
		Permission_OwnerWrite   |
		Permission_OwnerExecute |
		Permission_GroupRead    |
		Permission_GroupWrite   |
		Permission_GroupExecute |
		Permission_OthersRead   |
		Permission_OthersExecute, recursive);
}

static inline overload sdef(void, Create, RdString path, int mode) {
	scall(Create, path, mode, false);
}

static inline overload sdef(void, Create, RdString path) {
	scall(Create, path, false);
}

static inline overload sdef(void, SetTime, RdString path, time_t timestamp, bool followSymlink) {
	scall(SetTime, path, timestamp, 0, followSymlink);
}

static inline overload sdef(void, SetTime, RdString path, time_t timestamp) {
	scall(SetTime, path, timestamp, 0, false);
}

static inline overload sdef(String, expand, RdString path) {
	return scall(expand, path, false);
}

#undef self
