#include "Path.h"

Exception_Define(Path_AccessDeniedException);
Exception_Define(Path_AlreadyExistsException);
Exception_Define(Path_CreationFailedException);
Exception_Define(Path_EmptyPathException);
Exception_Define(Path_InsufficientSpaceException);
Exception_Define(Path_IsDirectoryException);
Exception_Define(Path_NameTooLongException);
Exception_Define(Path_NonExistentPathException);
Exception_Define(Path_NotDirectoryException);
Exception_Define(Path_ResolvingFailedException);
Exception_Define(Path_StatFailedException);
Exception_Define(Path_TruncatingFailedException);

static ExceptionManager *exc;

void Path0(ExceptionManager *e) {
	exc = e;
}

bool Path_Exists(String path) {
	return access(String_ToNul(&path), F_OK) == 0;
}

String Path_GetCwd(void) {
	String s = HeapString(512);
	ssize_t len;

	if ((len = syscall(SYS_getcwd, s.buf, s.size)) > 0) {
		s.len = len - 1;
	}

	return s;
}

struct stat64 Path_GetStat(String path) {
	if (path.len == 0) {
		throw(exc, &Path_EmptyPathException);
	}

	errno = 0;

	struct stat64 attr;

	if (stat64(String_ToNul(&path), &attr) == -1) {
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
	return Path_GetStat(path).st_size;
}

bool OVERLOAD Path_IsFile(String path) {
	return Path_GetStat(path).st_mode & S_IFREG;
}

bool OVERLOAD Path_IsFile(struct stat64 attr) {
	return attr.st_mode & S_IFREG;
}

bool OVERLOAD Path_IsDirectory(String path) {
	bool res = false;

	try (exc) {
		res = Path_GetStat(path).st_mode & S_IFDIR;
	} catch (&Path_NonExistentPathException, e) {
		res = false;
	} catch (&Path_NotDirectoryException, e) {
		res = false;
	} finally {

	} tryEnd;

	return res;
}

bool OVERLOAD Path_IsDirectory(struct stat64 attr) {
	return attr.st_mode & S_IFDIR;
}

void OVERLOAD Path_Truncate(String path, off64_t length) {
	if (path.len == 0) {
		throw(exc, &Path_EmptyPathException);
	}

	errno = 0;

	if (truncate64(String_ToNul(&path), length) == -1) {
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

void OVERLOAD Path_Truncate(String path) {
	Path_Truncate(path, 0);
}

String OVERLOAD Path_GetFilename(String path, bool verify) {
	if (path.len == 0) {
		throw(exc, &Path_EmptyPathException);
	}

	if (verify && !Path_IsFile(path)) {
		return String("");
	}

	ssize_t pos = String_ReverseFindChar(&path, '/');

	if (pos == String_NotFound) {
		return String_Clone(path);
	}

	if ((size_t) pos + 1 >= path.len) {
		return String_Clone(path);
	}

	return String_Slice(&path, pos + 1);
}

String OVERLOAD Path_GetFilename(String path) {
	return Path_GetFilename(path, true);
}

String OVERLOAD Path_GetDirectory(String path, bool verify) {
	if (path.len == 0) {
		throw(exc, &Path_EmptyPathException);
	}

	if (String_Equals(&path, String("/"))) {
		return String_Clone(String("/"));
	}

	if (String_EndsWith(&path, String("/"))) {
		return String_Slice(&path, 0, -1);
	}

	if (verify && Path_IsDirectory(path)) {
		return String_Clone(path);
	}

	ssize_t pos = String_ReverseFindChar(&path, '/');

	if (pos == String_NotFound) {
		return String_Clone(String("."));
	}

	return String_Slice(&path, 0, pos);
}

String OVERLOAD Path_GetDirectory(String path) {
	return Path_GetDirectory(path, true);
}

/* Modeled after http://insanecoding.blogspot.com/2007/11/implementing-realpath-in-c.html */
String Path_Resolve(String path) {
	if (path.len == 0) {
		throw(exc, &Path_EmptyPathException);
	}

	int fd;

	if ((fd = open(".", O_RDONLY)) == -1) {
		throw(exc, &Path_ResolvingFailedException);
	}

	bool isDir = Path_IsDirectory(path);

	String dirpath = path;

	if (!isDir) {
		dirpath = Path_GetDirectory(path, false);
	}

	String res = HeapString(0);

	if (chdir(String_ToNul(&dirpath)) == 0) {
		res = Path_GetCwd();

		if (!isDir) {
			String_Append(&res, '/');
			String_Append(&res, Path_GetFilename(path, false));
		}

		fchdir(fd);
	}

	if (!isDir) {
		String_Destroy(&dirpath);
	}

	close(fd);

	return res;
}

void OVERLOAD Path_Create(String path, int mode, bool recursive) {
	if (path.len == 0) {
		throw(exc, &Path_EmptyPathException);
	}

	if (String_Equals(&path, String("."))) {
		return;
	}

	if (Path_Exists(path)) {
		throw(exc, &Path_AlreadyExistsException);
	}

	if (recursive) {
		for (size_t i = 0; i < path.len; i++) {
			if (path.buf[i] == '/' || i == path.len - 1) {
				String tmp = String_Slice(&path, 0, i + 1);

				errno = 0;

				int res = mkdir(String_ToNul(&tmp), mode);

				String_Destroy(&tmp);

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

		if (mkdir(String_ToNul(&path), mode) == -1) {
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

void OVERLOAD Path_Create(String path, bool recursive) {
	Path_Create(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH, recursive);
}

void OVERLOAD Path_Create(String path, int mode) {
	Path_Create(path, mode, false);
}

void OVERLOAD Path_Create(String path) {
	Path_Create(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH, false);
}
