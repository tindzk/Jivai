#include "File.h"

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

Exception_Define(File_AccessDeniedException);
Exception_Define(File_AlreadyExistsException);
Exception_Define(File_AttributeNonExistentException);
Exception_Define(File_BufferTooSmallException);
Exception_Define(File_CannotOpenFileException);
Exception_Define(File_GettingAttributeFailedException);
Exception_Define(File_InvalidFileDescriptorException);
Exception_Define(File_InvalidParameterException);
Exception_Define(File_IsDirectoryException);
Exception_Define(File_NotFoundException);
Exception_Define(File_NotReadableException);
Exception_Define(File_NotWritableException);
Exception_Define(File_ReadingFailedException);
Exception_Define(File_ReadingInterruptedException);
Exception_Define(File_SeekingFailedException);
Exception_Define(File_SettingAttributeFailedException);
Exception_Define(File_StatFailedException);
Exception_Define(File_TruncatingFailedException);
Exception_Define(File_WritingFailedException);
Exception_Define(File_WritingInterruptedException);

static ExceptionManager *exc;

void File0(ExceptionManager *e) {
	exc = e;
}

void File_Open(File *this, String path, int mode) {
	errno = 0;

	if ((this->fd = open(String_ToNul(path), mode, 0666)) == -1) {
		if (errno == EACCES) {
			throw(exc, &File_AccessDeniedException);
		} else if (errno == ENOENT) {
			throw(exc, &File_NotFoundException);
		} else if (errno == EISDIR) {
			throw(exc, &File_IsDirectoryException);
		} else if (errno == EEXIST) {
			throw(exc, &File_AlreadyExistsException);
		} else {
			throw(exc, &File_CannotOpenFileException);
		}
	}

	this->readable = false;
	this->writable = false;

	switch (mode & 3) {
		case O_RDWR:
			this->readable = true;
			this->writable = true;
			break;

		case O_RDONLY:
			this->readable = true;
			break;

		case O_WRONLY:
			this->writable = true;
			break;
	}
}

void File_Close(File *this) {
	close(this->fd);
}

void File_SetXattr(File *this, String name, String value) {
	if (fsetxattr(this->fd, String_ToNul(name), value.buf, value.len, 0) < 0) {
		throw(exc, &File_SettingAttributeFailedException);
	}
}

String OVERLOAD File_GetXattr(File *this, String name) {
	char *nname = String_ToNul(name);

	errno = 0;

	ssize_t size = fgetxattr(this->fd, nname, NULL, 0);

	if (size < 0) {
		if (errno == ENODATA) {
			throw(exc, &File_AttributeNonExistentException);
		} else {
			throw(exc, &File_GettingAttributeFailedException);
		}
	}

	String res = HeapString(size);

	if (fgetxattr(this->fd, nname, res.buf, res.size) < 0) {
		throw(exc, &File_GettingAttributeFailedException);
	}

	res.len = res.size;

	return res;
}

void OVERLOAD File_GetXattr(File *this, String name, String *value) {
	char *nname = String_ToNul(name);

	errno = 0;

	ssize_t size = fgetxattr(this->fd, nname, value->buf, value->size);

	if (size < 0) {
		if (errno == ENODATA) {
			throw(exc, &File_AttributeNonExistentException);
		} else if (errno == ERANGE) {
			throw(exc, &File_BufferTooSmallException);
		} else {
			throw(exc, &File_GettingAttributeFailedException);
		}
	}

	value->len = size;
}

void OVERLOAD File_Truncate(File *this, off64_t length) {
	errno = 0;

	if (syscall(SYS_ftruncate64, this->fd, length) == -1) {
		if (errno == EBADF) {
			throw(exc, &File_InvalidFileDescriptorException);
		} else if (errno == EACCES) {
			throw(exc, &File_NotWritableException);
		} else if (errno == EINVAL) {
			throw(exc, &File_InvalidParameterException);
		} else {
			throw(exc, &File_TruncatingFailedException);
		}
	}
}

void OVERLOAD File_Truncate(File *this) {
	File_Truncate(this, 0);
}

Stat64 File_GetStat(File *this) {
	errno = 0;

	Stat64 attr;

	if (syscall(SYS_fstat64, this->fd, &attr) == -1) {
		if (errno == EACCES) {
			throw(exc, &File_AccessDeniedException);
		} else if (errno == EBADF) {
			throw(exc, &File_InvalidFileDescriptorException);
		} else {
			throw(exc, &File_StatFailedException);
		}
	}

	return attr;
}

off64_t File_GetSize(File *this) {
	return File_GetStat(this).size;
}

size_t File_Read(File *this, void *buf, size_t len) {
	if (!this->readable) {
		throw(exc, &File_NotReadableException);
	}

	errno = 0;

	ssize_t res;

	if ((res = read(this->fd, buf, len)) == -1) {
		if (errno == EINTR) {
			throw(exc, &File_ReadingInterruptedException);
		} else if (errno == EISDIR) {
			throw(exc, &File_IsDirectoryException);
		} else {
			throw(exc, &File_ReadingFailedException);
		}
	}

	return res;
}

size_t OVERLOAD File_Write(File *this, void *buf, size_t len) {
	if (!this->writable) {
		throw(exc, &File_NotWritableException);
	}

	errno = 0;

	ssize_t res;

	if ((res = write(this->fd, buf, len)) == -1) {
		if (errno == EINTR) {
			throw(exc, &File_WritingInterruptedException);
		} else if (errno == EISDIR) {
			throw(exc, &File_IsDirectoryException);
		} else {
			throw(exc, &File_WritingFailedException);
		}
	}

	return res;
}

size_t OVERLOAD File_Write(File *this, String s) {
	return File_Write(this, s.buf, s.len);
}

off64_t File_Seek(File *this, off64_t offset, File_SeekType whence) {
	if (!this->readable) {
		throw(exc, &File_NotReadableException);
	}

	errno = 0;

	off64_t pos;

	/* Conversion taken from dietlibc-0.32/lib/lseek64.c */
	if (syscall(SYS__llseek,
		this->fd,
		(unsigned long) (offset >> 32),
		(unsigned long) offset & 0xffffffff,
		&pos, whence) == -1)
	{
		if (errno == EBADF) {
			throw(exc, &File_InvalidFileDescriptorException);
		} else if (errno == EINVAL) {
			throw(exc, &File_InvalidParameterException);
		} else {
			throw(exc, &File_SeekingFailedException);
		}
	}

	return pos;
}

off64_t File_Tell(File *this) {
	return File_Seek(this, 0L, SEEK_CUR);
}
