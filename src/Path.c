#import "Path.h"

size_t Modules_Path;

static ExceptionManager *exc;

void Path0(ExceptionManager *e) {
	Modules_Path = Module_Register(String("Path"));

	exc = e;
}

overload bool Path_Exists(String path, bool follow) {
	Stat attr;
	return syscall(follow ? __NR_stat : __NR_lstat, String_ToNul(path), &attr) == 0;
}

inline overload bool Path_Exists(String path) {
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
		throw(exc, excEmptyPath);
	}

	errno = 0;

	Stat64 attr;

	if (syscall(__NR_stat64, String_ToNul(path), &attr) == -1) {
		if (errno == EACCES) {
			throw(exc, excAccessDenied);
		} else if (errno == ENAMETOOLONG) {
			throw(exc, excNameTooLong);
		} else if (errno == ENOENT) {
			throw(exc, excNonExistentPath);
		} else if (errno == ENOTDIR) {
			throw(exc, excNotDirectory);
		} else {
			throw(exc, excStatFailed);
		}
	}

	return attr;
}

off64_t Path_GetSize(String path) {
	return Path_GetStat(path).size;
}

inline overload bool Path_IsFile(String path) {
	return Path_GetStat(path).mode & FileMode_Regular;
}

inline overload bool Path_IsFile(Stat64 attr) {
	return attr.mode & FileMode_Regular;
}

overload bool Path_IsDirectory(String path) {
	bool res = false;

	try (exc) {
		res = Path_GetStat(path).mode & FileMode_Directory;
	} catch (Modules_Path, excNonExistentPath, e) {
		res = false;
	} catch (Modules_Path, excNotDirectory, e) {
		res = false;
	} finally {

	} tryEnd;

	return res;
}

inline overload bool Path_IsDirectory(Stat64 attr) {
	return attr.mode & FileMode_Directory;
}

overload void Path_Truncate(String path, off64_t length) {
	if (path.len == 0) {
		throw(exc, excEmptyPath);
	}

	errno = 0;

	if (syscall(__NR_truncate64, String_ToNul(path), length) == -1) {
		if (errno == EACCES) {
			throw(exc, excAccessDenied);
		} else if (errno == ENAMETOOLONG) {
			throw(exc, excNameTooLong);
		} else if (errno == ENOTDIR) {
			throw(exc, excNonExistentPath);
		} else if (errno == EISDIR) {
			throw(exc, excIsDirectory);
		} else {
			throw(exc, excTruncatingFailed);
		}
	}
}

inline overload void Path_Truncate(String path) {
	Path_Truncate(path, 0);
}

overload String Path_GetFilename(String path, bool verify) {
	if (path.len == 0) {
		throw(exc, excEmptyPath);
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

inline overload String Path_GetFilename(String path) {
	return Path_GetFilename(path, true);
}

String overload Path_GetDirectory(String path, bool verify) {
	if (path.len == 0) {
		throw(exc, excEmptyPath);
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

inline overload String Path_GetDirectory(String path) {
	return Path_GetDirectory(path, true);
}

/* Modeled after http://insanecoding.blogspot.com/2007/11/implementing-realpath-in-c.html */
String Path_Resolve(String path) {
	if (path.len == 0) {
		throw(exc, excEmptyPath);
	}

	int fd;

	if ((fd = syscall(__NR_open, ".", FileStatus_ReadOnly)) == -1) {
		throw(exc, excResolvingFailed);
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

overload void Path_Create(String path, int mode, bool recursive) {
	if (path.len == 0) {
		throw(exc, excEmptyPath);
	}

	if (String_Equals(path, String("."))) {
		return;
	}

	if (Path_Exists(path)) {
		throw(exc, excAlreadyExists);
	}

	if (recursive) {
		for (size_t i = 0; i < path.len; i++) {
			if (path.buf[i] == '/' || i == path.len - 1) {
				String tmp = String_Slice(path, 0, i + 1);

				errno = 0;

				int res = syscall(__NR_mkdir, String_ToNul(tmp), mode);

				if (res == -1) {
					if (errno == EACCES) {
						throw(exc, excAccessDenied);
					} else if (errno == ENAMETOOLONG) {
						throw(exc, excNameTooLong);
					} else if (errno == ENOSPC) {
						throw(exc, excInsufficientSpace);
					} else if (errno == ENOTDIR) {
						throw(exc, excNotDirectory);
					} else if (errno != EEXIST) {
						throw(exc, excCreationFailed);
					}
				}
			}
		}
	} else {
		errno = 0;

		if (syscall(__NR_mkdir, String_ToNul(path), mode) == -1) {
			if (errno == EACCES) {
				throw(exc, excAccessDenied);
			} else if (errno == EEXIST) {
				throw(exc, excAlreadyExists);
			} else if (errno == ENAMETOOLONG) {
				throw(exc, excNameTooLong);
			} else if (errno == ENOSPC) {
				throw(exc, excInsufficientSpace);
			} else if (errno == ENOTDIR) {
				throw(exc, excNotDirectory);
			} else {
				throw(exc, excCreationFailed);
			}
		}
	}
}

inline overload void Path_Create(String path, bool recursive) {
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

inline overload void Path_Create(String path, int mode) {
	Path_Create(path, mode, false);
}

inline overload void Path_Create(String path) {
	Path_Create(path, false);
}

void Path_Delete(String path) {
	errno = 0;

	if (syscall(__NR_unlink, String_ToNul(path)) == -1) {
		if (errno == EACCES) {
			throw(exc, excAccessDenied);
		} else if (errno == ENAMETOOLONG) {
			throw(exc, excNameTooLong);
		} else if (errno == ENOTDIR) {
			throw(exc, excNonExistentPath);
		} else if (errno == EISDIR) {
			throw(exc, excIsDirectory);
		} else {
			throw(exc, excDeletingFailed);
		}
	}
}

void Path_DeleteDirectory(String path) {
	errno = 0;

	if (syscall(__NR_rmdir, String_ToNul(path)) == -1) {
		if (errno == EACCES) {
			throw(exc, excAccessDenied);
		} else if (errno == ENAMETOOLONG) {
			throw(exc, excNameTooLong);
		} else if (errno == ENOTDIR) {
			throw(exc, excNotDirectory);
		} else if (errno == ENOENT) {
			throw(exc, excNonExistentPath);
		} else if (errno == ENOTEMPTY) {
			throw(exc, excDirectoryNotEmpty);
		} else {
			throw(exc, excDeletingFailed);
		}
	}
}

void Path_ReadLink(String path, String *out) {
	errno = 0;

	ssize_t len = syscall(__NR_readlink, String_ToNul(path), out->buf, out->size);

	if (len == -1) {
		if (errno == EACCES) {
			throw(exc, excAccessDenied);
		} else if (errno == ENAMETOOLONG) {
			throw(exc, excNameTooLong);
		} else if (errno == ENOTDIR) {
			throw(exc, excNonExistentPath);
		} else if (errno == ENOENT) {
			throw(exc, excNonExistentFile);
		} else {
			throw(exc, excReadingLinkFailed);
		}
	}

	out->len = len;
}

void Path_Symlink(String path1, String path2) {
	errno = 0;

	if (syscall(__NR_symlink, String_ToNul(path1), String_ToNul(path2)) == -1) {
		if (errno == EEXIST) {
			throw(exc, excAlreadyExists);
		} else {
			throw(exc, excCreationFailed);
		}
	}
}

void Path_SetXattr(String path, String name, String value) {
	if (syscall(__NR_setxattr, String_ToNul(path), String_ToNul(name), value.buf, value.len, 0) < 0) {
		throw(exc, excSettingAttributeFailed);
	}
}

overload String Path_GetXattr(String path, String name) {
	char *npath = String_ToNul(path);
	char *nname = String_ToNul(name);

	errno = 0;

	ssize_t size = syscall(__NR_getxattr, npath, nname, NULL, 0);

	if (size < 0) {
		if (errno == ENODATA) {
			throw(exc, excAttributeNonExistent);
		} else {
			throw(exc, excGettingAttributeFailed);
		}
	}

	String res = HeapString(size);

	if (getxattr(npath, nname, res.buf, res.size) < 0) {
		throw(exc, excGettingAttributeFailed);
	}

	res.len = res.size;

	return res;
}

overload void Path_GetXattr(String path, String name, String *value) {
	char *npath = String_ToNul(path);
	char *nname = String_ToNul(name);

	errno = 0;

	ssize_t size = syscall(__NR_getxattr, npath, nname, value->buf, value->size);

	if (size < 0) {
		if (errno == ENODATA) {
			throw(exc, excAttributeNonExistent);
		} else if (errno == ERANGE) {
			throw(exc, excBufferTooSmall);
		} else {
			throw(exc, excGettingAttributeFailed);
		}
	}

	value->len = size;
}

overload void Path_SetTime(String path, time_t timestamp, long nano, bool followSymlink) {
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
			throw(exc, excNameTooLong);
		} else if (errno == ENOENT) {
			throw(exc, excNonExistentPath);
		} else if (errno == ENOTDIR) {
			throw(exc, excNotDirectory);
		} else if (errno == EACCES) {
			throw(exc, excAccessDenied);
		} else if (errno == EPERM) {
			throw(exc, excPermissionDenied);
		} else {
			throw(exc, excSettingTimeFailed);
		}
	}
}

inline overload void Path_SetTime(String path, time_t timestamp, bool followSymlink) {
	Path_SetTime(path, timestamp, 0, followSymlink);
}

inline overload void Path_SetTime(String path, time_t timestamp) {
	Path_SetTime(path, timestamp, 0, false);
}
