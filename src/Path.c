#import "Path.h"

Exception_Define(AccessDeniedException);
Exception_Define(AlreadyExistsException);
Exception_Define(AttributeNonExistentException);
Exception_Define(BufferTooSmallException);
Exception_Define(CreationFailedException);
Exception_Define(DeletingFailedException);
Exception_Define(DirectoryNotEmptyException);
Exception_Define(EmptyPathException);
Exception_Define(GettingAttributeFailedException);
Exception_Define(InsufficientSpaceException);
Exception_Define(IsDirectoryException);
Exception_Define(NameTooLongException);
Exception_Define(NonExistentFileException);
Exception_Define(NonExistentPathException);
Exception_Define(NotDirectoryException);
Exception_Define(PermissionDeniedException);
Exception_Define(ReadingLinkFailedException);
Exception_Define(ResolvingFailedException);
Exception_Define(SettingAttributeFailedException);
Exception_Define(SettingTimeFailedException);
Exception_Define(StatFailedException);
Exception_Define(TruncatingFailedException);

static ExceptionManager *exc;

void Path0(ExceptionManager *e) {
	exc = e;
}

bool OVERLOAD Path_Exists(String path, bool follow) {
	Stat attr;
	return syscall(follow ? __NR_stat : __NR_lstat, String_ToNul(path), &attr) == 0;
}

inline bool OVERLOAD Path_Exists(String path) {
	return Path_Exists(path, false);
}

String Path_GetCwd(void) {
	String s = HeapString(512);
	ssize_t len;

	if ((len = syscall(__NR_getcwd, s.buf, s.size)) > 0) {
		s.len = len - 1;
	}

	return s;
}

Stat64 Path_GetStat(String path) {
	if (path.len == 0) {
		throw(exc, &EmptyPathException);
	}

	errno = 0;

	Stat64 attr;

	if (syscall(__NR_stat64, String_ToNul(path), &attr) == -1) {
		if (errno == EACCES) {
			throw(exc, &AccessDeniedException);
		} else if (errno == ENAMETOOLONG) {
			throw(exc, &NameTooLongException);
		} else if (errno == ENOENT) {
			throw(exc, &NonExistentPathException);
		} else if (errno == ENOTDIR) {
			throw(exc, &NotDirectoryException);
		} else {
			throw(exc, &StatFailedException);
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
	} catch (&NonExistentPathException, e) {
		res = false;
	} catch (&NotDirectoryException, e) {
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
		throw(exc, &EmptyPathException);
	}

	errno = 0;

	if (syscall(__NR_truncate64, String_ToNul(path), length) == -1) {
		if (errno == EACCES) {
			throw(exc, &AccessDeniedException);
		} else if (errno == ENAMETOOLONG) {
			throw(exc, &NameTooLongException);
		} else if (errno == ENOTDIR) {
			throw(exc, &NonExistentPathException);
		} else if (errno == EISDIR) {
			throw(exc, &IsDirectoryException);
		} else {
			throw(exc, &TruncatingFailedException);
		}
	}
}

inline void OVERLOAD Path_Truncate(String path) {
	Path_Truncate(path, 0);
}

String OVERLOAD Path_GetFilename(String path, bool verify) {
	if (path.len == 0) {
		throw(exc, &EmptyPathException);
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
		throw(exc, &EmptyPathException);
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
		throw(exc, &EmptyPathException);
	}

	int fd;

	if ((fd = syscall(__NR_open, ".", FileStatus_ReadOnly)) == -1) {
		throw(exc, &ResolvingFailedException);
	}

	bool isDir = Path_IsDirectory(path);

	String dirpath = !isDir
		? Path_GetDirectory(path, false)
		: path;

	String res = HeapString(0);

	if (syscall(__NR_chdir, String_ToNul(dirpath)) == 0) {
		res = Path_GetCwd();

		if (!isDir) {
			String_Append(&res, '/');
			String_Append(&res, Path_GetFilename(path, false));
		}

		syscall(__NR_fchdir, fd);
	}

	syscall(__NR_close, fd);

	return res;
}

void OVERLOAD Path_Create(String path, int mode, bool recursive) {
	if (path.len == 0) {
		throw(exc, &EmptyPathException);
	}

	if (String_Equals(path, String("."))) {
		return;
	}

	if (Path_Exists(path)) {
		throw(exc, &AlreadyExistsException);
	}

	if (recursive) {
		for (size_t i = 0; i < path.len; i++) {
			if (path.buf[i] == '/' || i == path.len - 1) {
				String tmp = String_Slice(path, 0, i + 1);

				errno = 0;

				int res = syscall(__NR_mkdir, String_ToNul(tmp), mode);

				if (res == -1) {
					if (errno == EACCES) {
						throw(exc, &AccessDeniedException);
					} else if (errno == ENAMETOOLONG) {
						throw(exc, &NameTooLongException);
					} else if (errno == ENOSPC) {
						throw(exc, &InsufficientSpaceException);
					} else if (errno == ENOTDIR) {
						throw(exc, &NotDirectoryException);
					} else if (errno != EEXIST) {
						throw(exc, &CreationFailedException);
					}
				}
			}
		}
	} else {
		errno = 0;

		if (syscall(__NR_mkdir, String_ToNul(path), mode) == -1) {
			if (errno == EACCES) {
				throw(exc, &AccessDeniedException);
			} else if (errno == EEXIST) {
				throw(exc, &AlreadyExistsException);
			} else if (errno == ENAMETOOLONG) {
				throw(exc, &NameTooLongException);
			} else if (errno == ENOSPC) {
				throw(exc, &InsufficientSpaceException);
			} else if (errno == ENOTDIR) {
				throw(exc, &NotDirectoryException);
			} else {
				throw(exc, &CreationFailedException);
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

	if (syscall(__NR_unlink, String_ToNul(path)) == -1) {
		if (errno == EACCES) {
			throw(exc, &AccessDeniedException);
		} else if (errno == ENAMETOOLONG) {
			throw(exc, &NameTooLongException);
		} else if (errno == ENOTDIR) {
			throw(exc, &NonExistentPathException);
		} else if (errno == EISDIR) {
			throw(exc, &IsDirectoryException);
		} else {
			throw(exc, &DeletingFailedException);
		}
	}
}

void Path_DeleteDirectory(String path) {
	errno = 0;

	if (syscall(__NR_rmdir, String_ToNul(path)) == -1) {
		if (errno == EACCES) {
			throw(exc, &AccessDeniedException);
		} else if (errno == ENAMETOOLONG) {
			throw(exc, &NameTooLongException);
		} else if (errno == ENOTDIR) {
			throw(exc, &NotDirectoryException);
		} else if (errno == ENOENT) {
			throw(exc, &NonExistentPathException);
		} else if (errno == ENOTEMPTY) {
			throw(exc, &DirectoryNotEmptyException);
		} else {
			throw(exc, &DeletingFailedException);
		}
	}
}

void Path_ReadLink(String path, String *out) {
	errno = 0;

	ssize_t len = syscall(__NR_readlink, String_ToNul(path), out->buf, out->size);

	if (len == -1) {
		if (errno == EACCES) {
			throw(exc, &AccessDeniedException);
		} else if (errno == ENAMETOOLONG) {
			throw(exc, &NameTooLongException);
		} else if (errno == ENOTDIR) {
			throw(exc, &NonExistentPathException);
		} else if (errno == ENOENT) {
			throw(exc, &NonExistentFileException);
		} else {
			throw(exc, &ReadingLinkFailedException);
		}
	}

	out->len = len;
}

void Path_Symlink(String path1, String path2) {
	errno = 0;

	if (syscall(__NR_symlink, String_ToNul(path1), String_ToNul(path2)) == -1) {
		if (errno == EEXIST) {
			throw(exc, &AlreadyExistsException);
		} else {
			throw(exc, &CreationFailedException);
		}
	}
}

void Path_SetXattr(String path, String name, String value) {
	if (syscall(__NR_setxattr, String_ToNul(path), String_ToNul(name), value.buf, value.len, 0) < 0) {
		throw(exc, &SettingAttributeFailedException);
	}
}

String OVERLOAD Path_GetXattr(String path, String name) {
	char *npath = String_ToNul(path);
	char *nname = String_ToNul(name);

	errno = 0;

	ssize_t size = syscall(__NR_getxattr, npath, nname, NULL, 0);

	if (size < 0) {
		if (errno == ENODATA) {
			throw(exc, &AttributeNonExistentException);
		} else {
			throw(exc, &GettingAttributeFailedException);
		}
	}

	String res = HeapString(size);

	if (getxattr(npath, nname, res.buf, res.size) < 0) {
		throw(exc, &GettingAttributeFailedException);
	}

	res.len = res.size;

	return res;
}

void OVERLOAD Path_GetXattr(String path, String name, String *value) {
	char *npath = String_ToNul(path);
	char *nname = String_ToNul(name);

	errno = 0;

	ssize_t size = syscall(__NR_getxattr, npath, nname, value->buf, value->size);

	if (size < 0) {
		if (errno == ENODATA) {
			throw(exc, &AttributeNonExistentException);
		} else if (errno == ERANGE) {
			throw(exc, &BufferTooSmallException);
		} else {
			throw(exc, &GettingAttributeFailedException);
		}
	}

	value->len = size;
}

void OVERLOAD Path_SetTime(String path, time_t timestamp, long nano, bool followSymlink) {
	Time_UnixEpoch t;

	t.sec  = timestamp;
	t.nsec = nano;

	int flags = !followSymlink ? AT_SYMLINK_NOFOLLOW : 0;

	errno = 0;

	if (syscall(__NR_utimensat, AT_FDCWD,
		String_ToNul(path),
		(const Time_UnixEpoch[2]) {t, t},
		flags) == -1)
	{
		if (errno == ENAMETOOLONG) {
			throw(exc, &NameTooLongException);
		} else if (errno == ENOENT) {
			throw(exc, &NonExistentPathException);
		} else if (errno == ENOTDIR) {
			throw(exc, &NotDirectoryException);
		} else if (errno == EACCES) {
			throw(exc, &AccessDeniedException);
		} else if (errno == EPERM) {
			throw(exc, &PermissionDeniedException);
		} else {
			throw(exc, &SettingTimeFailedException);
		}
	}
}

inline void OVERLOAD Path_SetTime(String path, time_t timestamp, bool followSymlink) {
	Path_SetTime(path, timestamp, 0, followSymlink);
}

inline void OVERLOAD Path_SetTime(String path, time_t timestamp) {
	Path_SetTime(path, timestamp, 0, false);
}
