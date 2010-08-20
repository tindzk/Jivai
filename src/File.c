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

Exception_Define(AccessDeniedException);
Exception_Define(AlreadyExistsException);
Exception_Define(AttributeNonExistentException);
Exception_Define(BufferTooSmallException);
Exception_Define(CannotOpenFileException);
Exception_Define(GettingAttributeFailedException);
Exception_Define(InvalidFileDescriptorException);
Exception_Define(InvalidParameterException);
Exception_Define(IsDirectoryException);
Exception_Define(NotFoundException);
Exception_Define(NotReadableException);
Exception_Define(NotWritableException);
Exception_Define(ReadingFailedException);
Exception_Define(ReadingInterruptedException);
Exception_Define(SeekingFailedException);
Exception_Define(SettingAttributeFailedException);
Exception_Define(StatFailedException);
Exception_Define(TruncatingFailedException);
Exception_Define(WritingFailedException);
Exception_Define(WritingInterruptedException);

static ExceptionManager *exc;

void File0(ExceptionManager *e) {
	exc = e;
}

void File_Open(File *this, String path, int mode) {
	errno = 0;

	if ((this->fd = syscall(__NR_open, String_ToNul(path), mode, 0666)) == -1) {
		if (errno == EACCES) {
			throw(exc, &AccessDeniedException);
		} else if (errno == ENOENT) {
			throw(exc, &NotFoundException);
		} else if (errno == EISDIR) {
			throw(exc, &IsDirectoryException);
		} else if (errno == EEXIST) {
			throw(exc, &AlreadyExistsException);
		} else {
			throw(exc, &CannotOpenFileException);
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
		throw(exc, &SettingAttributeFailedException);
	}
}

String OVERLOAD File_GetXattr(File *this, String name) {
	char *nname = String_ToNul(name);

	errno = 0;

	ssize_t size = syscall(__NR_fgetxattr, this->fd, nname, NULL, 0);

	if (size < 0) {
		if (errno == ENODATA) {
			throw(exc, &AttributeNonExistentException);
		} else {
			throw(exc, &GettingAttributeFailedException);
		}
	}

	String res = HeapString(size);

	if (syscall(__NR_fgetxattr, this->fd, nname, res.buf, res.size) < 0) {
		throw(exc, &GettingAttributeFailedException);
	}

	res.len = res.size;

	return res;
}

void OVERLOAD File_GetXattr(File *this, String name, String *value) {
	char *nname = String_ToNul(name);

	errno = 0;

	ssize_t size = syscall(__NR_fgetxattr, this->fd, nname, value->buf, value->size);

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

void OVERLOAD File_Truncate(File *this, off64_t length) {
	errno = 0;

	if (syscall(__NR_ftruncate64, this->fd, length) == -1) {
		if (errno == EBADF) {
			throw(exc, &InvalidFileDescriptorException);
		} else if (errno == EACCES) {
			throw(exc, &NotWritableException);
		} else if (errno == EINVAL) {
			throw(exc, &InvalidParameterException);
		} else {
			throw(exc, &TruncatingFailedException);
		}
	}
}

void OVERLOAD File_Truncate(File *this) {
	File_Truncate(this, 0);
}

Stat64 File_GetStat(File *this) {
	errno = 0;

	Stat64 attr;

	if (syscall(__NR_fstat64, this->fd, &attr) == -1) {
		if (errno == EACCES) {
			throw(exc, &AccessDeniedException);
		} else if (errno == EBADF) {
			throw(exc, &InvalidFileDescriptorException);
		} else {
			throw(exc, &StatFailedException);
		}
	}

	return attr;
}

off64_t File_GetSize(File *this) {
	return File_GetStat(this).size;
}

size_t File_Read(File *this, void *buf, size_t len) {
	if (!this->readable) {
		throw(exc, &NotReadableException);
	}

	errno = 0;

	ssize_t res;

	if ((res = syscall(__NR_read, this->fd, buf, len)) == -1) {
		if (errno == EINTR) {
			throw(exc, &ReadingInterruptedException);
		} else if (errno == EISDIR) {
			throw(exc, &IsDirectoryException);
		} else {
			throw(exc, &ReadingFailedException);
		}
	}

	return res;
}

size_t OVERLOAD File_Write(File *this, void *buf, size_t len) {
	if (!this->writable) {
		throw(exc, &NotWritableException);
	}

	errno = 0;

	ssize_t res;

	if ((res = write(this->fd, buf, len)) == -1) {
		if (errno == EINTR) {
			throw(exc, &WritingInterruptedException);
		} else if (errno == EISDIR) {
			throw(exc, &IsDirectoryException);
		} else {
			throw(exc, &WritingFailedException);
		}
	}

	return res;
}

size_t OVERLOAD File_Write(File *this, String s) {
	return File_Write(this, s.buf, s.len);
}

off64_t File_Seek(File *this, off64_t offset, File_SeekType whence) {
	if (!this->readable) {
		throw(exc, &NotReadableException);
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
			throw(exc, &InvalidFileDescriptorException);
		} else if (errno == EINVAL) {
			throw(exc, &InvalidParameterException);
		} else {
			throw(exc, &SeekingFailedException);
		}
	}

	return pos;
}

off64_t File_Tell(File *this) {
	return File_Seek(this, 0L, SEEK_CUR);
}
