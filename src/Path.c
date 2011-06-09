#import "Path.h"

#define self Path

overload sdef(bool, Exists, RdString path, bool follow) {
	Stat attr;

	if (follow) {
		return Kernel_stat(path, &attr);
	}

	return Kernel_lstat(path, &attr);
}

sdef(String, GetCwd) {
	String s = String_New(512);
	ssize_t len;

	if ((len = Kernel_getcwd(s.buf, 512)) > 0) {
		s.len = len - 1;
	}

	return s;
}

sdef(Stat64, GetStat, RdString path) {
	if (path.len == 0) {
		throw(EmptyPath);
	}

	Stat64 attr;

	if (!Kernel_stat64(path, &attr)) {
		if (errno == EACCES) {
			throw(AccessDenied);
		} else if (errno == ENAMETOOLONG) {
			throw(NameTooLong);
		} else if (errno == ENOENT) {
			throw(NonExistentPath);
		} else if (errno == ENOTDIR) {
			throw(NotDirectory);
		} else {
			throw(StatFailed);
		}
	}

	return attr;
}

sdef(u64, GetSize, RdString path) {
	return scall(GetStat, path).size;
}

overload sdef(bool, IsDirectory, RdString path) {
	bool res = false;

	try {
		res = scall(GetStat, path).mode & FileMode_Directory;
	} catch(Path, NonExistentPath) {
		/* Ignore. */
	} catch(Path, NotDirectory) {
		/* Ignore. */
	} finally {

	} tryEnd;

	return res;
}

overload sdef(void, Truncate, RdString path, u64 length) {
	if (path.len == 0) {
		throw(EmptyPath);
	}

	if (!Kernel_truncate64(path, length)) {
		if (errno == EACCES) {
			throw(AccessDenied);
		} else if (errno == ENAMETOOLONG) {
			throw(NameTooLong);
		} else if (errno == ENOTDIR) {
			throw(NonExistentPath);
		} else if (errno == EISDIR) {
			throw(IsDir);
		} else {
			throw(TruncatingFailed);
		}
	}
}

sdef(RdString, GetExtension, RdString path) {
	bwd(i, path.len) {
		if (path.buf[i] == '/') {
			break;
		}

		if (path.buf[i] == '.') {
			return String_Slice(path, i + 1);
		}
	}

	return $("");
}

overload sdef(RdString, GetFilename, RdString path, bool verify) {
	if (path.len == 0) {
		throw(EmptyPath);
	}

	if (verify && !scall(IsFile, path)) {
		return $("");
	}

	ssize_t pos = String_ReverseFind(path, '/');

	if (pos == String_NotFound) {
		return path;
	}

	if ((size_t) pos + 1 >= path.len) {
		return path;
	}

	return String_Slice(path, pos + 1);
}

overload sdef(RdString, GetDirectory, RdString path, bool verify) {
	if (path.len == 0) {
		throw(EmptyPath);
	}

	if (String_Equals(path, $("/"))) {
		return $("/");
	}

	if (String_EndsWith(path, $("/"))) {
		return String_Slice(path, 0, -1);
	}

	if (verify && scall(IsDirectory, path)) {
		return path;
	}

	ssize_t pos = String_ReverseFind(path, '/');

	if (pos == String_NotFound) {
		return $(".");
	}

	return String_Slice(path, 0, pos);
}

/* Modeled after http://insanecoding.blogspot.com/2007/11/implementing-realpath-in-c.html */
sdef(String, Resolve, RdString path) {
	if (path.len == 0) {
		throw(EmptyPath);
	}

	int fd;

	if ((fd = Kernel_open($("."), FileStatus_ReadOnly, 0)) == -1) {
		throw(ResolvingFailed);
	}

	bool isDir = scall(IsDirectory, path);

	RdString dirpath = !isDir
		? scall(GetDirectory, path, false)
		: path;

	String res = String_New(0);

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

overload sdef(void, Create, RdString path, int mode, bool recursive) {
	if (path.len == 0) {
		throw(EmptyPath);
	}

	if (String_Equals(path, $("."))) {
		return;
	}

	if (recursive) {
		fwd(i, path.len) {
			if (path.buf[i] == '/' || i == path.len - 1) {
				bool res = Kernel_mkdir(
					String_Slice(path, 0, i + 1),
					mode);

				if (!res) {
					if (errno == EACCES) {
						throw(AccessDenied);
					} else if (errno == ENAMETOOLONG) {
						throw(NameTooLong);
					} else if (errno == ENOSPC) {
						throw(InsufficientSpace);
					} else if (errno == ENOTDIR) {
						throw(NotDirectory);
					} else if (errno != EEXIST) {
						throw(CreationFailed);
					}
				}
			}
		}
	} else {
		if (!Kernel_mkdir(path, mode)) {
			if (errno == EACCES) {
				throw(AccessDenied);
			} else if (errno == EEXIST) {
				throw(AlreadyExists);
			} else if (errno == ENAMETOOLONG) {
				throw(NameTooLong);
			} else if (errno == ENOSPC) {
				throw(InsufficientSpace);
			} else if (errno == ENOTDIR) {
				throw(NotDirectory);
			} else {
				throw(CreationFailed);
			}
		}
	}
}

sdef(void, Delete, RdString path) {
	if (!Kernel_unlink(path)) {
		if (errno == EACCES) {
			throw(AccessDenied);
		} else if (errno == ENAMETOOLONG) {
			throw(NameTooLong);
		} else if (errno == ENOTDIR) {
			throw(NonExistentPath);
		} else if (errno == EISDIR) {
			throw(IsDir);
		} else {
			throw(DeletingFailed);
		}
	}
}

sdef(void, DeleteDirectory, RdString path) {
	if (!Kernel_rmdir(path)) {
		if (errno == EACCES) {
			throw(AccessDenied);
		} else if (errno == ENAMETOOLONG) {
			throw(NameTooLong);
		} else if (errno == ENOTDIR) {
			throw(NotDirectory);
		} else if (errno == ENOENT) {
			throw(NonExistentPath);
		} else if (errno == ENOTEMPTY) {
			throw(DirectoryNotEmpty);
		} else {
			throw(DeletingFailed);
		}
	}
}

sdef(void, ReadLink, RdString path, String *out) {
	ssize_t len = Kernel_readlink(path, out->buf, String_GetSize(*out));

	if (len == -1) {
		if (errno == EACCES) {
			throw(AccessDenied);
		} else if (errno == ENAMETOOLONG) {
			throw(NameTooLong);
		} else if (errno == ENOTDIR) {
			throw(NonExistentPath);
		} else if (errno == ENOENT) {
			throw(NonExistentFile);
		} else {
			throw(ReadingLinkFailed);
		}
	}

	out->len = len;
}

sdef(void, Symlink, RdString path1, RdString path2) {
	if (!Kernel_symlink(path1, path2)) {
		if (errno == EEXIST) {
			throw(AlreadyExists);
		} else {
			throw(CreationFailed);
		}
	}
}

sdef(void, SetXattr, RdString path, RdString name, RdString value) {
	if (!Kernel_setxattr(path, name, value.buf, value.len, 0)) {
		throw(SettingAttributeFailed);
	}
}

overload sdef(String, GetXattr, RdString path, RdString name) {
	ssize_t size = Kernel_getxattr(path, name, NULL, 0);

	if (size == -1) {
		if (errno == ENODATA) {
			throw(AttributeNonExistent);
		} else {
			throw(GettingAttributeFailed);
		}
	}

	String res = String_New(size);

	if (Kernel_getxattr(path, name, res.buf, size) == -1) {
		throw(GettingAttributeFailed);
	}

	res.len = size;

	return res;
}

overload sdef(void, GetXattr, RdString path, RdString name, String *value) {
	ssize_t size = Kernel_getxattr(path, name, value->buf, String_GetSize(*value));

	if (size == -1) {
		if (errno == ENODATA) {
			throw(AttributeNonExistent);
		} else if (errno == ERANGE) {
			throw(BufferTooSmall);
		} else {
			throw(GettingAttributeFailed);
		}
	}

	value->len = size;
}

overload sdef(void, SetTime, RdString path, time_t timestamp, long nano, bool followSymlink) {
	Time_UnixEpoch t = {
		.sec  = timestamp,
		.nsec = nano
	};

	int flags = !followSymlink ? AT_SYMLINK_NOFOLLOW : 0;

	if (!Kernel_utimensat(AT_FDCWD, path, t, flags)) {
		if (errno == ENAMETOOLONG) {
			throw(NameTooLong);
		} else if (errno == ENOENT) {
			throw(NonExistentPath);
		} else if (errno == ENOTDIR) {
			throw(NotDirectory);
		} else if (errno == EACCES) {
			throw(AccessDenied);
		} else if (errno == EPERM) {
			throw(PermissionDenied);
		} else {
			throw(SettingTimeFailed);
		}
	}
}
