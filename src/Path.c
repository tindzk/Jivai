#include "Path.h"

Exception_Define(Path_AccessDeniedException);
Exception_Define(Path_AlreadyExistsException);
Exception_Define(Path_AttributeNonExistentException);
Exception_Define(Path_BufferTooSmallException);
Exception_Define(Path_CreationFailedException);
Exception_Define(Path_DeletingFailedException);
Exception_Define(Path_DirectoryNotEmptyException);
Exception_Define(Path_EmptyPathException);
Exception_Define(Path_GettingAttributeFailedException);
Exception_Define(Path_InsufficientSpaceException);
Exception_Define(Path_IsDirectoryException);
Exception_Define(Path_NameTooLongException);
Exception_Define(Path_NonExistentFileException);
Exception_Define(Path_NonExistentPathException);
Exception_Define(Path_NotDirectoryException);
Exception_Define(Path_PermissionDeniedException);
Exception_Define(Path_ReadingLinkFailedException);
Exception_Define(Path_ResolvingFailedException);
Exception_Define(Path_SettingAttributeFailedException);
Exception_Define(Path_SettingTimeFailedException);
Exception_Define(Path_StatFailedException);
Exception_Define(Path_TruncatingFailedException);

static ExceptionManager *exc;

void Path0(ExceptionManager *e) {
	exc = e;
}

bool OVERLOAD Path_Exists(String path, bool follow) {
	Stat attr;
	return syscall(follow ? SYS_stat : SYS_lstat, String_ToNul(path), &attr) == 0;
}

inline bool OVERLOAD Path_Exists(String path) {
	return Path_Exists(path, false);
}

String Path_GetCwd(void) {
	String s = HeapString(512);
	ssize_t len;

	if ((len = syscall(SYS_getcwd, s.buf, s.size)) > 0) {
		s.len = len - 1;
	}

	return s;
}

Stat64 Path_GetStat(String path) {
	if (path.len == 0) {
		throw(exc, &Path_EmptyPathException);
	}

	errno = 0;

	Stat64 attr;

	if (syscall(SYS_stat64, String_ToNul(path), &attr) == -1) {
		if (errno == EACCES) {
			throw(exc, &Path_AccessDeniedException);
		} else if (errno == ENAMETOOLONG) {
			throw(exc, &Path_NameTooLongException);
		} else if (errno == ENOENT) {
			throw(exc, &Path_NonExistentPathException);
		} else if (errno == ENOTDIR) {
			throw(exc, &Path_NotDirectoryException);
		} else {
			throw(exc, &Path_StatFailedException);
		}
	}

	return attr;
}

off64_t Path_GetSize(String path) {
	return Path_GetStat(path).size;
}

inline bool OVERLOAD Path_IsFile(String path) {
	return Path_GetStat(path).mode & FileMode_Regular;
}

inline bool OVERLOAD Path_IsFile(Stat64 attr) {
	return attr.mode & FileMode_Regular;
}

bool OVERLOAD Path_IsDirectory(String path) {
	bool res = false;

	try (exc) {
		res = Path_GetStat(path).mode & FileMode_Directory;
	} catch (&Path_NonExistentPathException, e) {
		res = false;
	} catch (&Path_NotDirectoryException, e) {
		res = false;
	} finally {

	} tryEnd;

	return res;
}

inline bool OVERLOAD Path_IsDirectory(Stat64 attr) {
	return attr.mode & FileMode_Directory;
}

void OVERLOAD Path_Truncate(String path, off64_t length) {
	if (path.len == 0) {
		throw(exc, &Path_EmptyPathException);
	}

	errno = 0;

	if (syscall(SYS_truncate64, String_ToNul(path), length) == -1) {
		if (errno == EACCES) {
			throw(exc, &Path_AccessDeniedException);
		} else if (errno == ENAMETOOLONG) {
			throw(exc, &Path_NameTooLongException);
		} else if (errno == ENOTDIR) {
			throw(exc, &Path_NonExistentPathException);
		} else if (errno == EISDIR) {
			throw(exc, &Path_IsDirectoryException);
		} else {
			throw(exc, &Path_TruncatingFailedException);
		}
	}
}

inline void OVERLOAD Path_Truncate(String path) {
	Path_Truncate(path, 0);
}

String OVERLOAD Path_GetFilename(String path, bool verify) {
	if (path.len == 0) {
		throw(exc, &Path_EmptyPathException);
	}

	if (verify && !Path_IsFile(path)) {
		return String("");
	}

	ssize_t pos = String_ReverseFind(path, '/');

	path.mutable = false;

	if (pos == String_NotFound) {
		return path;
	}

	if ((size_t) pos + 1 >= path.len) {
		return path;
	}

	return String_Slice(path, pos + 1);
}

inline String OVERLOAD Path_GetFilename(String path) {
	return Path_GetFilename(path, true);
}

String OVERLOAD Path_GetDirectory(String path, bool verify) {
	if (path.len == 0) {
		throw(exc, &Path_EmptyPathException);
	}

	if (String_Equals(path, String("/"))) {
		return String("/");
	}

	if (String_EndsWith(path, String("/"))) {
		return String_Slice(path, 0, -1);
	}

	if (verify && Path_IsDirectory(path)) {
		path.mutable = false;
		return path;
	}

	ssize_t pos = String_ReverseFind(path, '/');

	if (pos == String_NotFound) {
		return String(".");
	}

	return String_Slice(path, 0, pos);
}

inline String OVERLOAD Path_GetDirectory(String path) {
	return Path_GetDirectory(path, true);
}

/* Modeled after http://insanecoding.blogspot.com/2007/11/implementing-realpath-in-c.html */
String Path_Resolve(String path) {
	if (path.len == 0) {
		throw(exc, &Path_EmptyPathException);
	}

	int fd;

	if ((fd = syscall(SYS_open, ".", FileStatus_ReadOnly)) == -1) {
		throw(exc, &Path_ResolvingFailedException);
	}

	bool isDir = Path_IsDirectory(path);

	String dirpath = !isDir
		? Path_GetDirectory(path, false)
		: path;

	String res = HeapString(0);

	if (syscall(SYS_chdir, String_ToNul(dirpath)) == 0) {
		res = Path_GetCwd();

		if (!isDir) {
			String_Append(&res, '/');
			String_Append(&res, Path_GetFilename(path, false));
		}

		syscall(SYS_fchdir, fd);
	}

	syscall(SYS_close, fd);

	return res;
}

void OVERLOAD Path_Create(String path, int mode, bool recursive) {
	if (path.len == 0) {
		throw(exc, &Path_EmptyPathException);
	}

	if (String_Equals(path, String("."))) {
		return;
	}

	if (Path_Exists(path)) {
		throw(exc, &Path_AlreadyExistsException);
	}

	if (recursive) {
		for (size_t i = 0; i < path.len; i++) {
			if (path.buf[i] == '/' || i == path.len - 1) {
				String tmp = String_Slice(path, 0, i + 1);

				errno = 0;

				int res = syscall(SYS_mkdir, String_ToNul(tmp), mode);

				if (res == -1) {
					if (errno == EACCES) {
						throw(exc, &Path_AccessDeniedException);
					} else if (errno == ENAMETOOLONG) {
						throw(exc, &Path_NameTooLongException);
					} else if (errno == ENOSPC) {
						throw(exc, &Path_InsufficientSpaceException);
					} else if (errno == ENOTDIR) {
						throw(exc, &Path_NotDirectoryException);
					} else if (errno != EEXIST) {
						throw(exc, &Path_CreationFailedException);
					}
				}
			}
		}
	} else {
		errno = 0;

		if (syscall(SYS_mkdir, String_ToNul(path), mode) == -1) {
			if (errno == EACCES) {
				throw(exc, &Path_AccessDeniedException);
			} else if (errno == EEXIST) {
				throw(exc, &Path_AlreadyExistsException);
			} else if (errno == ENAMETOOLONG) {
				throw(exc, &Path_NameTooLongException);
			} else if (errno == ENOSPC) {
				throw(exc, &Path_InsufficientSpaceException);
			} else if (errno == ENOTDIR) {
				throw(exc, &Path_NotDirectoryException);
			} else {
				throw(exc, &Path_CreationFailedException);
			}
		}
	}
}

inline void OVERLOAD Path_Create(String path, bool recursive) {
	Path_Create(path,
		Permission_OwnerRead    |
		Permission_OwnerWrite   |
		Permission_OwnerExecute |
		Permission_GroupRead    |
		Permission_GroupWrite   |
		Permission_GroupExecute |
		Permission_OthersRead   |
		Permission_OthersExecute, recursive);
}

inline void OVERLOAD Path_Create(String path, int mode) {
	Path_Create(path, mode, false);
}

inline void OVERLOAD Path_Create(String path) {
	Path_Create(path, false);
}

void Path_Delete(String path) {
	errno = 0;

	if (syscall(SYS_unlink, String_ToNul(path)) == -1) {
		if (errno == EACCES) {
			throw(exc, &Path_AccessDeniedException);
		} else if (errno == ENAMETOOLONG) {
			throw(exc, &Path_NameTooLongException);
		} else if (errno == ENOTDIR) {
			throw(exc, &Path_NonExistentPathException);
		} else if (errno == EISDIR) {
			throw(exc, &Path_IsDirectoryException);
		} else {
			throw(exc, &Path_DeletingFailedException);
		}
	}
}

void Path_DeleteDirectory(String path) {
	errno = 0;

	if (syscall(SYS_rmdir, String_ToNul(path)) == -1) {
		if (errno == EACCES) {
			throw(exc, &Path_AccessDeniedException);
		} else if (errno == ENAMETOOLONG) {
			throw(exc, &Path_NameTooLongException);
		} else if (errno == ENOTDIR) {
			throw(exc, &Path_NotDirectoryException);
		} else if (errno == ENOENT) {
			throw(exc, &Path_NonExistentPathException);
		} else if (errno == ENOTEMPTY) {
			throw(exc, &Path_DirectoryNotEmptyException);
		} else {
			throw(exc, &Path_DeletingFailedException);
		}
	}
}

void Path_ReadLink(String path, String *out) {
	errno = 0;

	ssize_t len = syscall(SYS_readlink, String_ToNul(path), out->buf, out->size);

	if (len == -1) {
		if (errno == EACCES) {
			throw(exc, &Path_AccessDeniedException);
		} else if (errno == ENAMETOOLONG) {
			throw(exc, &Path_NameTooLongException);
		} else if (errno == ENOTDIR) {
			throw(exc, &Path_NonExistentPathException);
		} else if (errno == ENOENT) {
			throw(exc, &Path_NonExistentFileException);
		} else {
			throw(exc, &Path_ReadingLinkFailedException);
		}
	}

	out->len = len;
}

void Path_Symlink(String path1, String path2) {
	errno = 0;

	if (syscall(SYS_symlink, String_ToNul(path1), String_ToNul(path2)) == -1) {
		if (errno == EEXIST) {
			throw(exc, &Path_AlreadyExistsException);
		} else {
			throw(exc, &Path_CreationFailedException);
		}
	}
}

void Path_SetXattr(String path, String name, String value) {
	if (syscall(SYS_setxattr, String_ToNul(path), String_ToNul(name), value.buf, value.len, 0) < 0) {
		throw(exc, &Path_SettingAttributeFailedException);
	}
}

String OVERLOAD Path_GetXattr(String path, String name) {
	char *npath = String_ToNul(path);
	char *nname = String_ToNul(name);

	errno = 0;

	ssize_t size = syscall(SYS_getxattr, npath, nname, NULL, 0);

	if (size < 0) {
		if (errno == ENODATA) {
			throw(exc, &Path_AttributeNonExistentException);
		} else {
			throw(exc, &Path_GettingAttributeFailedException);
		}
	}

	String res = HeapString(size);

	if (getxattr(npath, nname, res.buf, res.size) < 0) {
		throw(exc, &Path_GettingAttributeFailedException);
	}

	res.len = res.size;

	return res;
}

void OVERLOAD Path_GetXattr(String path, String name, String *value) {
	char *npath = String_ToNul(path);
	char *nname = String_ToNul(name);

	errno = 0;

	ssize_t size = syscall(SYS_getxattr, npath, nname, value->buf, value->size);

	if (size < 0) {
		if (errno == ENODATA) {
			throw(exc, &Path_AttributeNonExistentException);
		} else if (errno == ERANGE) {
			throw(exc, &Path_BufferTooSmallException);
		} else {
			throw(exc, &Path_GettingAttributeFailedException);
		}
	}

	value->len = size;
}

void OVERLOAD Path_SetTime(String path, time_t timestamp, long nano, bool followSymlink) {
	Date_UnixTime t;

	t.sec  = timestamp;
	t.usec = nano;

	int flags = !followSymlink ? AT_SYMLINK_NOFOLLOW : 0;

	errno = 0;

	if (syscall(SYS_utimensat, AT_FDCWD,
		String_ToNul(path),
		(const Date_UnixTime[2]) {t, t},
		flags) == -1)
	{
		if (errno == ENAMETOOLONG) {
			throw(exc, &Path_NameTooLongException);
		} else if (errno == ENOENT) {
			throw(exc, &Path_NonExistentPathException);
		} else if (errno == ENOTDIR) {
			throw(exc, &Path_NotDirectoryException);
		} else if (errno == EACCES) {
			throw(exc, &Path_AccessDeniedException);
		} else if (errno == EPERM) {
			throw(exc, &Path_PermissionDeniedException);
		} else {
			throw(exc, &Path_SettingTimeFailedException);
		}
	}
}

inline void OVERLOAD Path_SetTime(String path, time_t timestamp, bool followSymlink) {
	Path_SetTime(path, timestamp, 0, followSymlink);
}

inline void OVERLOAD Path_SetTime(String path, time_t timestamp) {
	Path_SetTime(path, timestamp, 0, false);
}
