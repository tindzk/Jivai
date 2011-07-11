#import "Path.h"

#define self Path

/* A path can refer either to a folder or a file. A valid path mustn't
 * be empty. It can be relative (to the current folder), or absolute when
 * it's the full path. Whether a path is absolute or relative is
 * indicated in its prefix: A leading slash (standing for the root
 * level) makes a path absolute, the lack thereof, relative.
 *
 * A file name has a "base" and an "extension". The latter is not
 * obligatory and can be safely omitted. All file names not containing
 * slashes are considered valid, except for "." and ".." which are
 * reserved names.
 *
 * Although the kernel doesn't require it (and some system calls even
 * reject such input), all folder paths must end with a slash. This also
 * applies to links!
 *
 * A link can point to an absolute or relative path. Links may also
 * point to paths that are invalid.  A link cannot be considered as a
 * file nor as a folder, it is more of a "superimposition" placed on top
 * of these two. Therefore links can only be created using createLink()
 * and deleted using deleteLink(). Links have their own meta information
 * such as permissions, modification dates etc. This is why most methods
 * provide a `follow' parameter which, when set to false, allows to work
 * on the link itself rather than the resolved path. Note that all methods
 * have this parameter set to true by default.
 *
 * isFolder(), isFile(), isLink() assume that the provided path is valid
 * and exists on the disk. If you're unsure, you must call Path_exists()
 * prior to any of these functions. Otherwise, you'd run into undefined
 * behaviour when compiling your application without assertions which
 * disable most checks.
 *
 * isAbsolutePath(), isFolderPath(), isFilePath(), isCreatableFolderPath()
 * and isCreatableFilePath() are methods to check the legality of the
 * given path. As no validation is performed, these functions work with
 * inexistant paths.
 *
 * getFileExtension(), getFileName(), getFolderName(), getFolderPath()
 * work on inexistant paths as well. However, as for most methods, the
 * provided paths must be still legal, i.e., use isFolderPath() or
 * isFilePath() when in doubt.
 *
 * If you cannot trust the user input, you will have two perform, both a
 * check for legality and one for validity, for example, verifying a
 * folder path, which in this case may even a link, may look like:
 *
 *  if (Path_isFolderPath(userInput) && Path_exists(userInput)) { ... }
 *
 * (The legality check has the sole purpose to determine the correct type
 * of a path, i.e., file or directory, whereas the validity check verifies
 * the existance in the file system.)
 *
 * If you only want to deal with valid folder links, write this instead:
 *
 *  if (Path_isFolderPath(userInput) && Path_isLink(userInput) &&
 *      Path_exists(userInput)) { ... }
 *
 * If you want to deal with either an invalid or a valid folder link,
 * add the parameter "false" to Path_exists() as this wouldn't follow the
 * link and thus completely ignore the path it's pointing to.
 *
 * Thanks to the flexible interface, a variety of other checks can be
 * performed easily without sacrificing readability.
 *
 * Note: All methods are very strict about its input. In many cases the
 * checks are redundant and may impact the performance negatively.
 * Therefore, a production build should be preferably not shipped with
 * assertions enabled.
 */

inline sdef(bool, isPath, RdString path) {
	return path.len != 0
		&& !String_Contains(path, $("//"));
}

sdef(bool, isAbsolutePath, RdString path) {
	assert(path.len != 0);
	return scall(isPath, path)
		&& path.buf[0] == '/';
}

sdef(bool, isFolderPath, RdString path) {
	return scall(isPath, path)
		&& path.buf[path.len - 1] == '/';
}

sdef(bool, isFilePath, RdString path) {
	return scall(isPath, path)
		&& path.buf[path.len - 1] != '/'
		&& !String_Equals(path, $("."))
		&& !String_Equals(path, $(".."));
}

sdef(bool, isCreatableFolderPath, RdString path) {
	return scall(isPath, path)
		&& path.buf[path.len - 1] == '/'
		&& !String_Equals(path, $("."))
		&& !String_Equals(path, $(".."));
}

sdef(bool, isCreatableFilePath, RdString path) {
	return scall(isPath, path)
		&& path.buf[path.len - 1] != '/'
		&& !String_Equals(path, $("."))
		&& !String_Equals(path, $(".."))
		&& !String_EndsWith(path, $("/."))
		&& !String_EndsWith(path, $("/.."));
}

overload sdef(Stat64, getMeta, RdString path, bool follow) {
	assert(scall(isFolderPath, path) || scall(isFilePath, path));
	assert(scall(exists, path, follow));

	if (scall(isFolderPath, path)) {
		/* Fix up path for system call. This is required when dealing
		 * with links.
		 */
		path.len--;
	}

	Stat64 attr;

	bool res = follow
		? Kernel_stat64(path, &attr)
		: Kernel_lstat64(path, &attr);

	if (!res) {
		if (errno == EACCES) {
			throw(AccessDenied);
		} else if (errno == ENAMETOOLONG) {
			throw(NameTooLong);
		} else if (errno == ENOENT) {
			throw(NonExistentPath);
		} else if (errno == ENOTDIR) {
			/* A component of the path is not a folder. */
			throw(NotFolder);
		} else {
			throw(UnknownError);
		}
	}

	return attr;
}

/* Checks whether the given path is valid, i.e. reachable. */
overload sdef(bool, exists, RdString path, bool follow) {
	assert(scall(isFolderPath, path) || scall(isFilePath, path));

	bool folder = (path.buf[path.len - 1] == '/');

	Stat meta;

	if (follow) {
		Kernel_stat(path, &meta) || ret(false);

		if ((meta.mode & FileMode_Mask) == FileMode_Folder) {
			folder || ret(false);
		} else {
			!folder || ret(false);
		}

		return true;
	} else {
		RdString realPath = path;
		if (folder) {
			realPath.len--;
		}

		bool status = Kernel_lstat(realPath, &meta) || ret(false);

		if ((meta.mode & FileMode_Mask) == FileMode_Link) {
			String target = String_New(256);
			ssize_t len = Kernel_readlink(realPath, target.buf, String_GetSize(target));
			if (len == -1) {
				throw(UnknownError);
			}
			target.len = len;
			assert(len != 0);

			bool folderTarget = (target.buf[target.len - 1] == '/');

			status = (folder == folderTarget);

			String_Destroy(&target);
		} else {
			if ((meta.mode & FileMode_Mask) == FileMode_Folder) {
				folder || ret(false);
			} else {
				!folder || ret(false);
			}
		}

		return status;
	}
}

/* Returns the path of the current working folder. */
sdef(String, getCurrent) {
	String s = String_New(512);
	ssize_t len = Kernel_getcwd(s.buf, String_GetSize(s));

	if (len == -1) {
		throw(UnknownError);
	}

	assert(len != 0);
	s.len = len - 1;

	if (s.len == 1) {
		assert(s.buf[0] == '/');
	} else {
		String_Append(&s, '/');
	}

	return s;
}

overload sdef(bool, isFolder, RdString path, bool follow) {
	assert(scall(isFolderPath, path));
	assert(scall(exists, path, follow));

	return (scall(getMeta, path, follow).mode & FileMode_Mask) == FileMode_Folder;
}

overload sdef(bool, isFile, RdString path, bool follow) {
	assert(scall(isFilePath, path));
	assert(scall(exists, path, follow));

	return (scall(getMeta, path, follow).mode & FileMode_Mask) == FileMode_Regular;
}

overload sdef(bool, isLink, RdString path) {
	assert(scall(isFolderPath, path) || scall(isFilePath, path));
	assert(scall(exists, path, false));

	return (scall(getMeta, path, false).mode & FileMode_Mask) == FileMode_Link;
}

overload sdef(void, truncate, RdString path, u64 length) {
	assert(scall(isFilePath, path));
	assert(scall(exists, path, true));
	assert(scall(isFile, path, true));

	if (!Kernel_truncate64(path, length)) {
		if (errno == EACCES) {
			throw(AccessDenied);
		} else if (errno == ENAMETOOLONG) {
			throw(NameTooLong);
		} else {
			throw(UnknownError);
		}
	}
}

sdef(RdString, getFileExtension, RdString path) {
	assert(scall(isFilePath, path));

	bwd(i, path.len) {
		if (path.buf[i] == '/') {
			break;
		} else if (path.buf[i] == '.') {
			return String_Slice(path, i + 1);
		}
	}

	return $("");
}

sdef(RdString, getFileName, RdString path) {
	assert(scall(isFilePath, path));

	ssize_t pos = String_ReverseFind(path, '/');

	if (pos == String_NotFound) {
		return path;
	} else if ((size_t) pos + 1 >= path.len) {
		return path;
	}

	return String_Slice(path, pos + 1);
}

sdef(RdString, getFolderName, RdString path) {
	assert(scall(isFolderPath, path));

	path.len--; /* Slash. */

	ssize_t pos = String_ReverseFind(path, '/');

	if (pos == String_NotFound) {
		return $(".");
	}

	return String_Slice(path, pos + 1);
}

sdef(RdString, getFolderPath, RdString path) {
	assert(scall(isFolderPath, path) || scall(isFilePath, path));

	ssize_t pos = String_ReverseFind(path, '/');

	if (pos == String_NotFound) {
		return $("./");
	}

	return String_Slice(path, 0, pos + 1);
}

/* Modeled after http://insanecoding.blogspot.com/2007/11/implementing-realpath-in-c.html */
sdef(String, expandFolder, RdString path) {
	assert(scall(isFolderPath, path));
	assert(scall(exists, path, true));
	assert(scall(isFolder, path, true));

	int fd = Kernel_open($("."), FileStatus_ReadOnly, 0);

	if (fd == -1) {
		throw(UnknownError);
	}

	if (!Kernel_chdir(path)) {
		Kernel_close(fd);
		throw(UnknownError);
	}

	String res = scall(getCurrent);
	assert(res.len != 0 && res.buf[res.len - 1] == '/');

	Kernel_fchdir(fd);
	Kernel_close(fd);

	return res;
}

sdef(String, expandFile, RdString path) {
	assert(scall(isFilePath, path));
	assert(scall(exists, path, true));
	assert(scall(isFile, path, true));

	int fd = Kernel_open($("."), FileStatus_ReadOnly, 0);

	if (fd == -1) {
		throw(UnknownError);
	}

	RdString folderPath = scall(getFolderPath, path);
	RdString fileName   = scall(getFileName,   path);

	if (!Kernel_chdir(folderPath)) {
		Kernel_close(fd);
		throw(UnknownError);
	}

	String res = scall(getCurrent);
	assert(res.len != 0 && res.buf[res.len - 1] == '/');
	String_Append(&res, fileName);

	Kernel_fchdir(fd);
	Kernel_close(fd);

	return res;
}

sdef(String, expand, RdString path) {
	assert(scall(isFolderPath, path) || scall(isFilePath, path));
	assert(scall(exists, path, true));

	if (scall(isFolderPath, path) && scall(isFolder, path, true)) {
		return scall(expandFolder, path);
	} else {
		return scall(expandFile, path);
	}
}

overload sdef(void, createFolder, RdString path, int mode, bool recursive) {
	assert(scall(isCreatableFolderPath, path));
	assert(!scall(exists, path, false));

	if (!recursive) {
		Kernel_mkdir(path, mode) || jmp(error);
	} else {
		fwd(i, path.len) {
			if (path.buf[i] != '/') {
				continue;
			}

			if (!Kernel_mkdir(String_Slice(path, 0, i + 1), mode)) {
				errno == EEXIST || jmp(error);
			}
		}
	}

	when (error) {
		if (errno == EACCES) {
			throw(AccessDenied);
		} else if (errno == ENAMETOOLONG) {
			throw(NameTooLong);
		} else if (errno == ENOSPC) {
			throw(InsufficientSpace);
		} else if (errno == ENOTDIR) {
			throw(NotFolder);
		} else if (errno == ENOENT) {
			throw(NonExistentPath);
		} else {
			throw(UnknownError);
		}
	}
}

sdef(void, deleteLink, RdString path) {
	assert(scall(isFolderPath, path) || scall(isFilePath, path));
	assert(scall(exists, path, false));

#if Exception_Assert
	Stat64 meta = scall(getMeta, path, false);

	assert(scall(isLink, meta));

	String target = scall(followLink, path);

	if (scall(isFolderPath, target.rd)) {
		assert(scall(isFolderPath, path));
	} else if (scall(isFilePath, target.rd)) {
		assert(scall(isFilePath, path));
	}

	String_Destroy(&target);
#endif

	RdString realPath = path;
	if (scall(isFolderPath, path)) {
		realPath.len--;
	}

	if (!Kernel_unlink(realPath)) {
		if (errno == EACCES) {
			throw(AccessDenied);
		} else if (errno == ENAMETOOLONG) {
			throw(NameTooLong);
		} else if (errno == ENOTDIR) {
			throw(NonExistentPath);
		} else if (errno == EISDIR) {
			throw(IsFolder);
		} else {
			throw(UnknownError);
		}
	}
}

overload sdef(void, deleteFile, RdString path, bool follow) {
	assert(scall(isFilePath, path));
	assert(scall(exists, path, follow));

	bool status;

	if (follow && scall(isLink, path)) {
		String target = scall(followLink, path);
		status = Kernel_unlink(target.rd);
		String_Destroy(&target);
	} else {
		assert(scall(isFile, path, follow));
		status = Kernel_unlink(path);
	}

	if (!status) {
		if (errno == EACCES) {
			throw(AccessDenied);
		} else if (errno == ENAMETOOLONG) {
			throw(NameTooLong);
		} else if (errno == ENOTDIR) {
			throw(NonExistentPath);
		} else if (errno == EISDIR) {
			throw(IsFolder);
		} else {
			throw(UnknownError);
		}
	}
}

sdef(void, deleteFolder, RdString path) {
	assert(scall(isFolderPath, path));
	assert(scall(exists, path, true));
	assert(scall(isFolder, path, true));

	if (!Kernel_rmdir(path)) {
		if (errno == EACCES) {
			throw(AccessDenied);
		} else if (errno == ENAMETOOLONG) {
			throw(NameTooLong);
		} else if (errno == ENOTDIR) {
			throw(NotFolder);
		} else if (errno == ENOENT) {
			throw(NonExistentPath);
		} else if (errno == ENOTEMPTY) {
			throw(FolderNotEmpty);
		} else {
			throw(UnknownError);
		}
	}
}

overload sdef(void, followLink, RdString path, String *out) {
	assert(scall(isFolderPath, path) || scall(isFilePath, path));
	assert(scall(exists, path, false));

	assert(scall(isLink, path));

	if (scall(isFolderPath, path)) {
		path.len--;
	}

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
			throw(UnknownError);
		}
	}

	out->len = len;
}

overload sdef(String, followLink, RdString path) {
	String res = String_New(256);
	scall(followLink, path, &res);
	return res;
}

sdef(void, createLink, RdString target, RdString path) {
	assert((scall(isFilePath,   target) && scall(isFilePath,   path)) ||
			scall(isFolderPath, target) && scall(isFolderPath, path));
	assert(!scall(exists, path, false));

	if (scall(isFolderPath, path)) {
		path.len--;
	}

	if (!Kernel_symlink(target, path)) {
		throw(UnknownError);
	}
}

sdef(void, setExtendedAttribute, RdString path, RdString name, RdString value) {
	assert(scall(isFolderPath, path) || scall(isFilePath, path));
	assert(scall(exists, path, true));

	if (!Kernel_setxattr(path, name, value.buf, value.len, 0)) {
		throw(UnknownError);
	}
}

overload sdef(String, getExtendedAttribute, RdString path, RdString name) {
	assert(scall(isFolderPath, path) || scall(isFilePath, path));
	assert(scall(exists, path, true));

	ssize_t size = Kernel_getxattr(path, name, NULL, 0);

	if (size == -1) {
		if (errno == ENODATA) {
			throw(AttributeNonExistent);
		} else {
			throw(UnknownError);
		}
	}

	String res = String_New(size);

	if (Kernel_getxattr(path, name, res.buf, size) == -1) {
		throw(UnknownError);
	}

	res.len = size;

	return res;
}

overload sdef(void, getExtendedAttribute, RdString path, RdString name, String *value) {
	assert(scall(isFolderPath, path) || scall(isFilePath, path));
	assert(scall(exists, path, true));

	ssize_t size = Kernel_getxattr(path, name, value->buf, String_GetSize(*value));

	if (size == -1) {
		if (errno == ENODATA) {
			throw(AttributeNonExistent);
		} else if (errno == ERANGE) {
			throw(BufferTooSmall);
		} else {
			throw(UnknownError);
		}
	}

	value->len = size;
}

/* Sets the "last access" and "last modification" time. */
overload sdef(void, setTime, RdString path, Time_UnixEpoch time, bool follow) {
	assert(scall(isFolderPath, path) || scall(isFilePath, path));
	assert(scall(exists, path, follow));

	int flags = !follow ? AT_SYMLINK_NOFOLLOW : 0;

	if (!Kernel_utimensat(AT_FDCWD, path, time, flags)) {
		if (errno == ENAMETOOLONG) {
			throw(NameTooLong);
		} else if (errno == ENOENT) {
			throw(NonExistentPath);
		} else if (errno == ENOTDIR) {
			throw(NotFolder);
		} else if (errno == EACCES) {
			throw(AccessDenied);
		} else if (errno == EPERM) {
			throw(PermissionDenied);
		} else {
			throw(UnknownError);
		}
	}
}

/* Returns the "last modification" time. */
sdef(Time_UnixEpoch, getTime, RdString path) {
	return scall(getMeta, path).mtime;
}

sdef(u64, getSize, RdString path) {
	return scall(getMeta, path).size;
}
