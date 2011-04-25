#import "File.h"

#define self File

rsdef(self, New, RdString path, int flags) {
	errno = 0;

	int id = Kernel_open(path, flags, 0666);

	if (id == -1) {
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

	return (self) {
		.ch = Channel_New(id, flags)
	};
}

def(void, Destroy) {
	Channel_Destroy(&this->ch);
}

def(void, SetXattr, RdString name, RdString value) {
	if (!Kernel_fsetxattr(Channel_GetId(&this->ch), name, value.buf, value.len, 0)) {
		throw(SettingAttributeFailed);
	}
}

overload def(String, GetXattr, RdString name) {
	errno = 0;

	ssize_t size = Kernel_fgetxattr(Channel_GetId(&this->ch), name, NULL, 0);

	if (size == -1) {
		if (errno == ENODATA) {
			throw(AttributeNonExistent);
		} else {
			throw(GettingAttributeFailed);
		}
	}

	String res = String_New(size);

	if (Kernel_fgetxattr(Channel_GetId(&this->ch), name, res.buf, size) == -1) {
		throw(GettingAttributeFailed);
	}

	res.len = size;

	return res;
}

overload def(void, GetXattr, RdString name, String *value) {
	errno = 0;

	ssize_t size = Kernel_fgetxattr(Channel_GetId(&this->ch), name, value->buf, String_GetSize(*value));

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

	if (!Kernel_ftruncate64(Channel_GetId(&this->ch), length)) {
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

	if (!Kernel_fstat64(Channel_GetId(&this->ch), &attr)) {
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

def(u64, Seek, u64 offset, ref(SeekType) whence) {
	assert(Channel_IsReadable(&this->ch));

	errno = 0;

	u64 pos;

	if (!Kernel_llseek(Channel_GetId(&this->ch), offset, &pos, whence)) {
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

overload def(size_t, Read, void *buf, size_t len) {
	return Channel_Read(&this->ch, buf, len);
}

overload def(void, Read, String *res) {
	return Channel_Read(&this->ch, res);
}

overload def(size_t, Write, char *buf, size_t len) {
	return Channel_Write(&this->ch, buf, len);
}

overload def(size_t, Write, RdString s) {
	return Channel_Write(&this->ch, s);
}

overload def(size_t, Write, char c) {
	return Channel_Write(&this->ch, c);
}

sdef(void, GetContents, RdString path, String *res) {
	File file = scall(New, path, FileStatus_ReadOnly);

	size_t len = 0;
	size_t size = String_GetSize(*res);

	do {
		len = scall(Read, &file,
			res->buf + res->len,
			size - res->len);

		res->len += len;
	} while (len > 0);
}
