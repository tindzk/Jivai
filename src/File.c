#import "File.h"

static File _File_StdIn = {
	.fd       = STDIN_FILENO,
	.readable = true,
	.writable = false
};

static File _File_StdOut = {
	.fd       = STDOUT_FILENO,
	.readable = false,
	.writable = true
};

static File _File_StdErr = {
	.fd       = STDERR_FILENO,
	.readable = false,
	.writable = true
};

File *File_StdIn  = &_File_StdIn;
File *File_StdOut = &_File_StdOut;
File *File_StdErr = &_File_StdErr;

size_t Modules_File;

static ExceptionManager *exc;

void File0(ExceptionManager *e) {
	Modules_File = Module_Register(String("File"));

	exc = e;
}

void File_Open(File *this, String path, int mode) {
	errno = 0;

	if ((this->fd = syscall(__NR_open, String_ToNul(path), mode, 0666)) == -1) {
		if (errno == EACCES) {
			throw(exc, excAccessDenied);
		} else if (errno == ENOENT) {
			throw(exc, excNotFound);
		} else if (errno == EISDIR) {
			throw(exc, excIsDirectory);
		} else if (errno == EEXIST) {
			throw(exc, excAlreadyExists);
		} else {
			throw(exc, excCannotOpenFile);
		}
	}

	this->readable = false;
	this->writable = false;

	switch (mode & 3) {
		case FileStatus_ReadWrite:
			this->readable = true;
			this->writable = true;
			break;

		case FileStatus_ReadOnly:
			this->readable = true;
			break;

		case FileStatus_WriteOnly:
			this->writable = true;
			break;
	}
}

void File_Close(File *this) {
	syscall(__NR_close, this->fd);
}

void File_SetXattr(File *this, String name, String value) {
	if (syscall(__NR_fsetxattr, this->fd, String_ToNul(name), value.buf, value.len, 0) < 0) {
		throw(exc, excSettingAttributeFailed);
	}
}

overload String File_GetXattr(File *this, String name) {
	char *nname = String_ToNul(name);

	errno = 0;

	ssize_t size = syscall(__NR_fgetxattr, this->fd, nname, NULL, 0);

	if (size < 0) {
		if (errno == ENODATA) {
			throw(exc, excAttributeNonExistent);
		} else {
			throw(exc, excGettingAttributeFailed);
		}
	}

	String res = HeapString(size);

	if (syscall(__NR_fgetxattr, this->fd, nname, res.buf, res.size) < 0) {
		throw(exc, excGettingAttributeFailed);
	}

	res.len = res.size;

	return res;
}

overload void File_GetXattr(File *this, String name, String *value) {
	char *nname = String_ToNul(name);

	errno = 0;

	ssize_t size = syscall(__NR_fgetxattr, this->fd, nname, value->buf, value->size);

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

overload void File_Truncate(File *this, off64_t length) {
	errno = 0;

	if (syscall(__NR_ftruncate64, this->fd, length) == -1) {
		if (errno == EBADF) {
			throw(exc, excInvalidFileDescriptor);
		} else if (errno == EACCES) {
			throw(exc, excNotWritable);
		} else if (errno == EINVAL) {
			throw(exc, excInvalidParameter);
		} else {
			throw(exc, excTruncatingFailed);
		}
	}
}

overload void File_Truncate(File *this) {
	File_Truncate(this, 0);
}

Stat64 File_GetStat(File *this) {
	errno = 0;

	Stat64 attr;

	if (syscall(__NR_fstat64, this->fd, &attr) == -1) {
		if (errno == EACCES) {
			throw(exc, excAccessDenied);
		} else if (errno == EBADF) {
			throw(exc, excInvalidFileDescriptor);
		} else {
			throw(exc, excStatFailed);
		}
	}

	return attr;
}

off64_t File_GetSize(File *this) {
	return File_GetStat(this).size;
}

size_t File_Read(File *this, void *buf, size_t len) {
	if (!this->readable) {
		throw(exc, excNotReadable);
	}

	errno = 0;

	ssize_t res;

	if ((res = syscall(__NR_read, this->fd, buf, len)) == -1) {
		if (errno == EINTR) {
			throw(exc, excReadingInterrupted);
		} else if (errno == EISDIR) {
			throw(exc, excIsDirectory);
		} else {
			throw(exc, excReadingFailed);
		}
	}

	return res;
}

overload size_t File_Write(File *this, void *buf, size_t len) {
	if (!this->writable) {
		throw(exc, excNotWritable);
	}

	errno = 0;

	ssize_t res;

	if ((res = write(this->fd, buf, len)) == -1) {
		if (errno == EINTR) {
			throw(exc, excWritingInterrupted);
		} else if (errno == EISDIR) {
			throw(exc, excIsDirectory);
		} else {
			throw(exc, excWritingFailed);
		}
	}

	return res;
}

overload size_t File_Write(File *this, String s) {
	return File_Write(this, s.buf, s.len);
}

off64_t File_Seek(File *this, off64_t offset, File_SeekType whence) {
	if (!this->readable) {
		throw(exc, excNotReadable);
	}

	errno = 0;

	off64_t pos;

	/* Conversion taken from dietlibc-0.32/lib/lseek64.c */
	if (syscall(__NR__llseek,
		this->fd,
		(unsigned long) (offset >> 32),
		(unsigned long) offset & 0xffffffff,
		&pos, whence) == -1)
	{
		if (errno == EBADF) {
			throw(exc, excInvalidFileDescriptor);
		} else if (errno == EINVAL) {
			throw(exc, excInvalidParameter);
		} else {
			throw(exc, excSeekingFailed);
		}
	}

	return pos;
}

off64_t File_Tell(File *this) {
	return File_Seek(this, 0L, File_SeekType_Cur);
}

void File_GetContents(String path, String *res) {
	File file;
	File_Open(&file, path, FileStatus_ReadOnly);

	size_t len = 0;

	do {
		len = File_Read(&file,
			res->buf  + res->len,
			res->size - res->len);

		res->len += len;
	} while (len > 0);
}
