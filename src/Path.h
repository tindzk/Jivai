#import "Time.h"
#import "Kernel.h"
#import "String.h"
#import "Exception.h"

#define self Path

exc(AccessDenied)
exc(AlreadyExists)
exc(AttributeNonExistent)
exc(BufferTooSmall)
exc(FolderNotEmpty)
exc(InsufficientSpace)
exc(IsFolder)
exc(NameTooLong)
exc(NonExistentFile)
exc(NonExistentPath)
exc(NotFolder)
exc(PermissionDenied)
exc(UnknownError)

#ifndef AT_FDCWD
#define AT_FDCWD -100
#endif

#ifndef AT_SYMLINK_NOFOLLOW
#define AT_SYMLINK_NOFOLLOW 0x100
#endif

sdef(bool, isAbsolutePath, RdString path);
sdef(bool, isFolderPath, RdString path);
sdef(bool, isFilePath, RdString path);
sdef(bool, isCreatableFolderPath, RdString path);
sdef(bool, isCreatableFilePath, RdString path);
overload sdef(Stat64, getMeta, RdString path, bool follow);
overload sdef(bool, exists, RdString path, bool follow);
sdef(String, getCurrent);
overload sdef(bool, isFolder, RdString path, bool follow);
overload sdef(bool, isFile, RdString path, bool follow);
overload sdef(bool, isLink, RdString path);
overload sdef(void, truncate, RdString path, u64 length);
sdef(RdString, getFileExtension, RdString path);
sdef(RdString, getFileName, RdString path);
sdef(RdString, getFolderName, RdString path);
sdef(RdString, getFolderPath, RdString path);
sdef(String, expandFolder, RdString path);
sdef(String, expandFile, RdString path);
sdef(String, expand, RdString path);
overload sdef(void, createFolder, RdString path, int mode, bool recursive);
sdef(void, deleteLink, RdString path);
overload sdef(void, deleteFile, RdString path, bool follow);
sdef(void, deleteFolder, RdString path);
overload sdef(void, followLink, RdString path, String *out);
overload sdef(String, followLink, RdString path);
sdef(void, createLink, RdString target, RdString path);
sdef(void, setExtendedAttribute, RdString path, RdString name, RdString value);
overload sdef(String, getExtendedAttribute, RdString path, RdString name);
overload sdef(void, getExtendedAttribute, RdString path, RdString name, String *value);
overload sdef(void, setTime, RdString path, Time_UnixEpoch time, bool follow);
sdef(Time_UnixEpoch, getTime, RdString path);
sdef(u64, getSize, RdString path);

static inline overload sdef(Stat64, getMeta, RdString path) {
	return scall(getMeta, path, true);
}

static inline overload sdef(bool, exists, RdString path) {
	return scall(exists, path, true);
}

static inline overload sdef(bool, isFile, RdString path) {
	return scall(isFile, path, true);
}

static inline overload sdef(bool, isFolder, RdString path) {
	return scall(isFolder, path, true);
}

static inline overload sdef(bool, isFile, Stat64 attr) {
	return (attr.mode & FileMode_Mask) == FileMode_Regular;
}

static inline overload sdef(bool, isFolder, Stat64 attr) {
	return (attr.mode & FileMode_Mask) == FileMode_Folder;
}

static inline overload sdef(bool, isLink, Stat64 attr) {
	return (attr.mode & FileMode_Mask) == FileMode_Link;
}

static inline overload sdef(void, deleteFile, RdString path) {
	scall(deleteFile, path, true);
}

static inline overload sdef(void, truncate, RdString path) {
	scall(truncate, path, 0);
}

static inline overload sdef(void, createFolder, RdString path, bool recursive) {
	scall(createFolder, path,
		Permission_OwnerRead    |
		Permission_OwnerWrite   |
		Permission_OwnerExecute |
		Permission_GroupRead    |
		Permission_GroupWrite   |
		Permission_GroupExecute |
		Permission_OthersRead   |
		Permission_OthersExecute, recursive);
}

static inline overload sdef(void, createFolder, RdString path, int mode) {
	scall(createFolder, path, mode, false);
}

static inline overload sdef(void, createFolder, RdString path) {
	scall(createFolder, path, false);
}

static inline overload sdef(void, setTime, RdString path, Time_UnixEpoch time) {
	scall(setTime, path, time, true);
}

#undef self
