#import "File.h"

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
			throw(excAccessDenied);
		} else if (errno == ENOENT) {
			throw(excNotFound);
		} else if (errno == EISDIR) {
			throw(excIsDirectory);
		} else if (errno == EEXIST) {
			throw(excAlreadyExists);
		} else {
			throw(excCannotOpenFile);
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
		throw(excSettingAttributeFailed);
	}
}

overload def(String, GetXattr, String name) {
	errno = 0;

	ssize_t size = Kernel_fgetxattr(this->fd, name, NULL, 0);

	if (size == -1) {
		if (errno == ENODATA) {
			throw(excAttributeNonExistent);
		} else {
			throw(excGettingAttributeFailed);
		}
	}

	String res = HeapString(size);

	if (Kernel_fgetxattr(this->fd, name, res.buf, res.size) == -1) {
		throw(excGettingAttributeFailed);
	}

	res.len = res.size;

	return res;
}

overload def(void, GetXattr, String name, String *value) {
	errno = 0;

	ssize_t size = Kernel_fgetxattr(this->fd, name, value->buf, value->size);

	if (size == -1) {
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

overload def(void, Truncate, u64 length) {
	errno = 0;

	if (!Kernel_ftruncate64(this->fd, length)) {
		if (errno == EBADF) {
			throw(excInvalidFileDescriptor);
		} else if (errno == EACCES) {
			throw(excNotWritable);
		} else if (errno == EINVAL) {
			throw(excInvalidParameter);
		} else {
			throw(excTruncatingFailed);
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
			throw(excAccessDenied);
		} else if (errno == EBADF) {
			throw(excInvalidFileDescriptor);
		} else {
			throw(excStatFailed);
		}
	}

	return attr;
}

def(u64, GetSize) {
	return call(GetStat).size;
}

overload def(size_t, Read, void *buf, size_t len) {
	if (!this->readable) {
		throw(excNotReadable);
	}

	errno = 0;

	ssize_t res;

	if ((res = Kernel_read(this->fd, buf, len)) == -1) {
		if (errno == EINTR) {
			throw(excReadingInterrupted);
		} else if (errno == EISDIR) {
			throw(excIsDirectory);
		} else {
			throw(excReadingFailed);
		}
	}

	return res;
}

inline overload def(void, Read, String *res) {
	res->len = call(Read, res->buf, res->size);
}

overload def(size_t, Write, void *buf, size_t len) {
	if (!this->writable) {
		throw(excNotWritable);
	}

	errno = 0;

	ssize_t res;

	if ((res = Kernel_write(this->fd, buf, len)) == -1) {
		if (errno == EINTR) {
			throw(excWritingInterrupted);
		} else if (errno == EISDIR) {
			throw(excIsDirectory);
		} else {
			throw(excWritingFailed);
		}
	}

	return res;
}

overload def(size_t, Write, String s) {
	return call(Write, s.buf, s.len);
}

def(u64, Seek, u64 offset, ref(SeekType) whence) {
	if (!this->readable) {
		throw(excNotReadable);
	}

	errno = 0;

	u64 pos;

	if (!Kernel_llseek(this->fd, offset, &pos, whence)) {
		if (errno == EBADF) {
			throw(excInvalidFileDescriptor);
		} else if (errno == EINVAL) {
			throw(excInvalidParameter);
		} else {
			throw(excSeekingFailed);
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
