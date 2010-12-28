#import "File.h"

#define self File

#undef File_Write
#undef File_Read

static self stdIn = {
	.fd       = FileNo_StdIn,
	.readable = true,
	.writable = false
};

static self stdOut = {
	.fd       = FileNo_StdOut,
	.readable = false,
	.writable = true
};

static self stdErr = {
	.fd       = FileNo_StdErr,
	.readable = false,
	.writable = true
};

self* ref(StdIn)  = &stdIn;
self* ref(StdOut) = &stdOut;
self* ref(StdErr) = &stdErr;

def(void, Open, String path, int mode) {
	errno = 0;

	if ((this->fd = Kernel_open(path, mode, 0666)) == -1) {
		if (errno == EACCES) {
			throw(AccessDenied);
		} else if (errno == ENOENT) {
			throw(NotFound);
		} else if (errno == EISDIR) {
			throw(IsDirectory);
		} else if (errno == EEXIST) {
			throw(AlreadyExists);
		} else {
			throw(CannotOpenFile);
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

def(void, Close) {
	Kernel_close(this->fd);
}

def(void, SetXattr, String name, String value) {
	if (!Kernel_fsetxattr(this->fd, name, value.buf, value.len, 0)) {
		throw(SettingAttributeFailed);
	}
}

overload def(String, GetXattr, String name) {
	errno = 0;

	ssize_t size = Kernel_fgetxattr(this->fd, name, NULL, 0);

	if (size == -1) {
		if (errno == ENODATA) {
			throw(AttributeNonExistent);
		} else {
			throw(GettingAttributeFailed);
		}
	}

	String res = HeapString(size);

	if (Kernel_fgetxattr(this->fd, name, res.buf, res.size) == -1) {
		throw(GettingAttributeFailed);
	}

	res.len = res.size;

	return res;
}

overload def(void, GetXattr, String name, String *value) {
	errno = 0;

	ssize_t size = Kernel_fgetxattr(this->fd, name, value->buf, value->size);

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

overload def(void, Truncate, u64 length) {
	errno = 0;

	if (!Kernel_ftruncate64(this->fd, length)) {
		if (errno == EBADF) {
			throw(InvalidFileDescriptor);
		} else if (errno == EACCES) {
			throw(NotWritable);
		} else if (errno == EINVAL) {
			throw(InvalidParameter);
		} else {
			throw(TruncatingFailed);
		}
	}
}

overload def(void, Truncate) {
	call(Truncate, 0);
}

def(Stat64, GetStat) {
	errno = 0;

	Stat64 attr;

	if (!Kernel_fstat64(this->fd, &attr)) {
		if (errno == EACCES) {
			throw(AccessDenied);
		} else if (errno == EBADF) {
			throw(InvalidFileDescriptor);
		} else {
			throw(StatFailed);
		}
	}

	return attr;
}

def(u64, GetSize) {
	return call(GetStat).size;
}

overload def(size_t, Read, void *buf, size_t len) {
	if (!this->readable) {
		throw(NotReadable);
	}

	errno = 0;

	ssize_t res;

	if ((res = Kernel_read(this->fd, buf, len)) == -1) {
		if (errno == EINTR) {
			throw(ReadingInterrupted);
		} else if (errno == EISDIR) {
			throw(IsDirectory);
		} else {
			throw(ReadingFailed);
		}
	}

	return res;
}

inline overload def(void, Read, String *res) {
	res->len = call(Read, res->buf, res->size);
}

overload def(size_t, Write, void *buf, size_t len) {
	if (!this->writable) {
		throw(NotWritable);
	}

	errno = 0;

	ssize_t res;

	if ((res = Kernel_write(this->fd, buf, len)) == -1) {
		if (errno == EINTR) {
			throw(WritingInterrupted);
		} else if (errno == EISDIR) {
			throw(IsDirectory);
		} else {
			throw(WritingFailed);
		}
	}

	return res;
}

overload def(size_t, Write, String s) {
	return call(Write, s.buf, s.len);
}

overload def(size_t, Write, char c) {
	return call(Write, &c, 1);
}

def(u64, Seek, u64 offset, ref(SeekType) whence) {
	if (!this->readable) {
		throw(NotReadable);
	}

	errno = 0;

	u64 pos;

	if (!Kernel_llseek(this->fd, offset, &pos, whence)) {
		if (errno == EBADF) {
			throw(InvalidFileDescriptor);
		} else if (errno == EINVAL) {
			throw(InvalidParameter);
		} else {
			throw(SeekingFailed);
		}
	}

	return pos;
}

def(u64, Tell) {
	return call(Seek, 0L, ref(SeekType_Cur));
}

void ref(GetContents)(String path, String *res) {
	File file;
	ref(Open)(&file, path, FileStatus_ReadOnly);

	size_t len = 0;

	do {
		len = ref(Read)(
			File_FromObject(&file),
			res->buf  + res->len,
			res->size - res->len);

		res->len += len;
	} while (len > 0);
}
