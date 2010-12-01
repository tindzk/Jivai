#import "Path.h"

#define self Path

overload sdef(bool, Exists, String path, bool follow) {
	Stat attr;

	if (follow) {
		return Kernel_stat(path, &attr);
	}

	return Kernel_lstat(path, &attr);
}

inline overload sdef(bool, Exists, String path) {
	return scall(Exists, path, false);
}

sdef(String, GetCwd) {
	String s = HeapString(512);
	ssize_t len;

	if ((len = Kernel_getcwd(s.buf, s.size)) > 0) {
		s.len = len - 1;
	}

	return s;
}

sdef(Stat64, GetStat, String path) {
	if (path.len == 0) {
		throw(excEmptyPath);
	}

	errno = 0;

	Stat64 attr;

	if (!Kernel_stat64(path, &attr)) {
		if (errno == EACCES) {
			throw(excAccessDenied);
		} else if (errno == ENAMETOOLONG) {
			throw(excNameTooLong);
		} else if (errno == ENOENT) {
			throw(excNonExistentPath);
		} else if (errno == ENOTDIR) {
			throw(excNotDirectory);
		} else {
			throw(excStatFailed);
		}
	}

	return attr;
}

sdef(u64, GetSize, String path) {
	return scall(GetStat, path).size;
}

inline overload sdef(bool, IsFile, String path) {
	return scall(GetStat, path).mode & FileMode_Regular;
}

inline overload sdef(bool, IsFile, Stat64 attr) {
	return attr.mode & FileMode_Regular;
}

overload sdef(bool, IsDirectory, String path) {
	bool res = false;

	try {
		res = scall(GetStat, path).mode & FileMode_Directory;
	} clean catch(Path, excNonExistentPath) {
		/* Ignore. */
	} catch(Path, excNotDirectory) {
		/* Ignore. */
	} finally {

	} tryEnd;

	return res;
}

inline overload sdef(bool, IsDirectory, Stat64 attr) {
	return attr.mode & FileMode_Directory;
}

overload sdef(void, Truncate, String path, u64 length) {
	if (path.len == 0) {
		throw(excEmptyPath);
	}

	errno = 0;

	if (!Kernel_truncate64(path, length)) {
		if (errno == EACCES) {
			throw(excAccessDenied);
		} else if (errno == ENAMETOOLONG) {
			throw(excNameTooLong);
		} else if (errno == ENOTDIR) {
			throw(excNonExistentPath);
		} else if (errno == EISDIR) {
			throw(excIsDirectory);
		} else {
			throw(excTruncatingFailed);
		}
	}
}

inline overload sdef(void, Truncate, String path) {
	scall(Truncate, path, 0);
}

overload sdef(String, GetFilename, String path, bool verify) {
	if (path.len == 0) {
		throw(excEmptyPath);
	}

	if (verify && !scall(IsFile, path)) {
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

inline overload sdef(String, GetFilename, String path) {
	return scall(GetFilename, path, true);
}

overload sdef(String, GetDirectory, String path, bool verify) {
	if (path.len == 0) {
		throw(excEmptyPath);
	}

	if (String_Equals(path, String("/"))) {
		return String("/");
	}

	if (String_EndsWith(path, String("/"))) {
		return String_Slice(path, 0, -1);
	}

	if (verify && scall(IsDirectory, path)) {
		path.mutable = false;
		return path;
	}

	ssize_t pos = String_ReverseFind(path, '/');

	if (pos == String_NotFound) {
		return String(".");
	}

	return String_Slice(path, 0, pos);
}

inline overload sdef(String, GetDirectory, String path) {
	return scall(GetDirectory, path, true);
}

/* Modeled after http://insanecoding.blogspot.com/2007/11/implementing-realpath-in-c.html */
sdef(String, Resolve, String path) {
	if (path.len == 0) {
		throw(excEmptyPath);
	}

	int fd;

	if ((fd = Kernel_open(String("."), FileStatus_ReadOnly, 0)) == -1) {
		throw(excResolvingFailed);
	}

	bool isDir = scall(IsDirectory, path);

	String dirpath = !isDir
		? scall(GetDirectory, path, false)
		: path;

	String res = HeapString(0);

	if (Kernel_chdir(dirpath)) {
		res = scall(GetCwd);

		if (!isDir) {
			String_Append(&res, '/');
			String_Append(&res, scall(GetFilename, path, false));
		}

		Kernel_fchdir(fd);
	}

	Kernel_close(fd);

	return res;
}

overload sdef(void, Create, String path, int mode, bool recursive) {
	if (path.len == 0) {
		throw(excEmptyPath);
	}

	if (String_Equals(path, String("."))) {
		return;
	}

	if (scall(Exists, path)) {
		throw(excAlreadyExists);
	}

	if (recursive) {
		for (size_t i = 0; i < path.len; i++) {
			if (path.buf[i] == '/' || i == path.len - 1) {
				errno = 0;

				bool res = Kernel_mkdir(
					String_Slice(path, 0, i + 1),
					mode);

				if (!res) {
					if (errno == EACCES) {
						throw(excAccessDenied);
					} else if (errno == ENAMETOOLONG) {
						throw(excNameTooLong);
					} else if (errno == ENOSPC) {
						throw(excInsufficientSpace);
					} else if (errno == ENOTDIR) {
						throw(excNotDirectory);
					} else if (errno != EEXIST) {
						throw(excCreationFailed);
					}
				}
			}
		}
	} else {
		errno = 0;

		if (!Kernel_mkdir(path, mode)) {
			if (errno == EACCES) {
				throw(excAccessDenied);
			} else if (errno == EEXIST) {
				throw(excAlreadyExists);
			} else if (errno == ENAMETOOLONG) {
				throw(excNameTooLong);
			} else if (errno == ENOSPC) {
				throw(excInsufficientSpace);
			} else if (errno == ENOTDIR) {
				throw(excNotDirectory);
			} else {
				throw(excCreationFailed);
			}
		}
	}
}

inline overload sdef(void, Create, String path, bool recursive) {
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

inline overload sdef(void, Create, String path, int mode) {
	scall(Create, path, mode, false);
}

inline overload sdef(void, Create, String path) {
	scall(Create, path, false);
}

sdef(void, Delete, String path) {
	errno = 0;

	if (!Kernel_unlink(path)) {
		if (errno == EACCES) {
			throw(excAccessDenied);
		} else if (errno == ENAMETOOLONG) {
			throw(excNameTooLong);
		} else if (errno == ENOTDIR) {
			throw(excNonExistentPath);
		} else if (errno == EISDIR) {
			throw(excIsDirectory);
		} else {
			throw(excDeletingFailed);
		}
	}
}

sdef(void, DeleteDirectory, String path) {
	errno = 0;

	if (!Kernel_rmdir(path)) {
		if (errno == EACCES) {
			throw(excAccessDenied);
		} else if (errno == ENAMETOOLONG) {
			throw(excNameTooLong);
		} else if (errno == ENOTDIR) {
			throw(excNotDirectory);
		} else if (errno == ENOENT) {
			throw(excNonExistentPath);
		} else if (errno == ENOTEMPTY) {
			throw(excDirectoryNotEmpty);
		} else {
			throw(excDeletingFailed);
		}
	}
}

sdef(void, ReadLink, String path, String *out) {
	errno = 0;

	ssize_t len = Kernel_readlink(path, out->buf, out->size);

	if (len == -1) {
		if (errno == EACCES) {
			throw(excAccessDenied);
		} else if (errno == ENAMETOOLONG) {
			throw(excNameTooLong);
		} else if (errno == ENOTDIR) {
			throw(excNonExistentPath);
		} else if (errno == ENOENT) {
			throw(excNonExistentFile);
		} else {
			throw(excReadingLinkFailed);
		}
	}

	out->len = len;
}

sdef(void, Symlink, String path1, String path2) {
	errno = 0;

	if (!Kernel_symlink(path1, path2)) {
		if (errno == EEXIST) {
			throw(excAlreadyExists);
		} else {
			throw(excCreationFailed);
		}
	}
}

sdef(void, SetXattr, String path, String name, String value) {
	if (!Kernel_setxattr(path, name, value.buf, value.len, 0)) {
		throw(excSettingAttributeFailed);
	}
}

overload sdef(String, GetXattr, String path, String name) {
	errno = 0;

	ssize_t size = Kernel_getxattr(path, name, NULL, 0);

	if (size == -1) {
		if (errno == ENODATA) {
			throw(excAttributeNonExistent);
		} else {
			throw(excGettingAttributeFailed);
		}
	}

	String res = HeapString(size);

	if (Kernel_getxattr(path, name, res.buf, res.size) == -1) {
		throw(excGettingAttributeFailed);
	}

	res.len = res.size;

	return res;
}

overload sdef(void, GetXattr, String path, String name, String *value) {
	errno = 0;

	ssize_t size = Kernel_getxattr(path, name, value->buf, value->size);

	if (size < 0) {
		if (errno == ENODATA) {
			throw(excAttributeNonExistent);
		} else if (errno == ERANGE) {
			throw(excBufferTooSmall);
		} else {
			throw(excGettingAttributeFailed);
		}
	}

	value->len = size;
}

overload sdef(void, SetTime, String path, time_t timestamp, long nano, bool followSymlink) {
	Time_UnixEpoch t;

	t.sec  = timestamp;
	t.nsec = nano;

	int flags = !followSymlink ? AT_SYMLINK_NOFOLLOW : 0;

	errno = 0;

	if (!Kernel_utimensat(AT_FDCWD, path, t, flags)) {
		if (errno == ENAMETOOLONG) {
			throw(excNameTooLong);
		} else if (errno == ENOENT) {
			throw(excNonExistentPath);
		} else if (errno == ENOTDIR) {
			throw(excNotDirectory);
		} else if (errno == EACCES) {
			throw(excAccessDenied);
		} else if (errno == EPERM) {
			throw(excPermissionDenied);
		} else {
			throw(excSettingTimeFailed);
		}
	}
}

inline overload sdef(void, SetTime, String path, time_t timestamp, bool followSymlink) {
	scall(SetTime, path, timestamp, 0, followSymlink);
}

inline overload sdef(void, SetTime, String path, time_t timestamp) {
	scall(SetTime, path, timestamp, 0, false);
}
