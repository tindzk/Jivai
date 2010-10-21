#import "File.h"

static File _File_StdIn = {
	.fd       = FileNo_StdIn,
	.readable = true,
	.writable = false
};

static File _File_StdOut = {
	.fd       = FileNo_StdOut,
	.readable = false,
	.writable = true
};

static File _File_StdErr = {
	.fd       = FileNo_StdErr,
	.readable = false,
	.writable = true
};

File *File_StdIn  = &_File_StdIn;
File *File_StdOut = &_File_StdOut;
File *File_StdErr = &_File_StdErr;

static ExceptionManager *exc;

void File0(ExceptionManager *e) {
	exc = e;
}

void File_Open(File *this, String path, int mode) {
	errno = 0;

	if ((this->fd = Kernel_open(path, mode, 0666)) == -1) {
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
	Kernel_close(this->fd);
}

void File_SetXattr(File *this, String name, String value) {
	if (!Kernel_fsetxattr(this->fd, name, value.buf, value.len, 0)) {
		throw(exc, excSettingAttributeFailed);
	}
}

overload String File_GetXattr(File *this, String name) {
	errno = 0;

	ssize_t size = Kernel_fgetxattr(this->fd, name, NULL, 0);

	if (size == -1) {
		if (errno == ENODATA) {
			throw(exc, excAttributeNonExistent);
		} else {
			throw(exc, excGettingAttributeFailed);
		}
	}

	String res = HeapString(size);

	if (Kernel_fgetxattr(this->fd, name, res.buf, res.size) == -1) {
		throw(exc, excGettingAttributeFailed);
	}

	res.len = res.size;

	return res;
}

overload void File_GetXattr(File *this, String name, String *value) {
	errno = 0;

	ssize_t size = Kernel_fgetxattr(this->fd, name, value->buf, value->size);

	if (size == -1) {
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

overload void File_Truncate(File *this, u64 length) {
	errno = 0;

	if (!Kernel_ftruncate64(this->fd, length)) {
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

	if (!Kernel_fstat64(this->fd, &attr)) {
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

u64 File_GetSize(File *this) {
	return File_GetStat(this).size;
}

overload size_t File_Read(File *this, void *buf, size_t len) {
	if (!this->readable) {
		throw(exc, excNotReadable);
	}

	errno = 0;

	ssize_t res;

	if ((res = Kernel_read(this->fd, buf, len)) == -1) {
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

inline overload void File_Read(File *this, String *res) {
	res->len = File_Read(this, res->buf, res->size);
}

overload size_t File_Write(File *this, void *buf, size_t len) {
	if (!this->writable) {
		throw(exc, excNotWritable);
	}

	errno = 0;

	ssize_t res;

	if ((res = Kernel_write(this->fd, buf, len)) == -1) {
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

u64 File_Seek(File *this, u64 offset, File_SeekType whence) {
	if (!this->readable) {
		throw(exc, excNotReadable);
	}

	errno = 0;

	u64 pos;

	if (!Kernel_llseek(this->fd, offset, &pos, whence)) {
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

u64 File_Tell(File *this) {
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
