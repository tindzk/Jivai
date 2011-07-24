#import "File.h"

#define self File

rsdef(self, new, RdString path, int flags) {
	assert(Path_isFilePath(path));

	__unused bool c = (flags & FileStatus_Create) != 0;
	__unused bool e = Path_exists(path);

	assert((!e && c) || e);

	int id = Kernel_open(path, flags, 0666);

	if (id == -1) {
		if (errno == EACCES) {
			throw(AccessDenied);
		} else if (errno == ENOENT) {
			throw(NotFound);
		} else if (errno == EISDIR) {
			throw(IsFolder);
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

def(void, destroy) {
	Channel_Destroy(&this->ch);
}

def(void, setExtendedAttribute, RdString name, RdString value) {
	if (!Kernel_fsetxattr(Channel_GetId(&this->ch), name, value.buf, value.len, 0)) {
		throw(SettingAttributeFailed);
	}
}

overload def(String, getExtendedAttribute, RdString name) {
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

overload def(void, getExtendedAttribute, RdString name, String *value) {
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

overload def(void, truncate, u64 length) {
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

def(Stat64, getMeta) {
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

def(u64, seek, u64 offset, ref(SeekType) whence) {
	assert(Channel_IsReadable(&this->ch));

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

def(u64, tell) {
	return call(seek, 0L, ref(SeekType_Cur));
}

overload def(size_t, read, void *buf, size_t len) {
	return Channel_Read(&this->ch, buf, len);
}

overload def(void, read, String *res) {
	return Channel_Read(&this->ch, res);
}

overload def(size_t, write, void *buf, size_t len) {
	return Channel_Write(&this->ch, buf, len);
}

overload def(size_t, write, RdString s) {
	return Channel_Write(&this->ch, s);
}

overload def(size_t, write, char c) {
	return Channel_Write(&this->ch, c);
}

overload sdef(void, getContents, RdString path, String *res) {
	File file = scall(new, path, FileStatus_ReadOnly);

	size_t len = 0;
	size_t size = String_GetSize(*res);

	do {
		len = scall(read, &file,
			res->buf + res->len,
			size - res->len);

		res->len += len;
	} while (len > 0);

	scall(destroy, &file);
}

overload sdef(String, getContents, RdString path) {
	File file = scall(new, path, FileStatus_ReadOnly);

	size_t len  = 0;
	u64    size = scall(getSize, &file);

	if (size == 0) {
		/* This is the case for /dev/stdin. */
		size = 4096;
	}

	assert(size <= MaxValue(size_t));

	String res = String_New((size_t) size);

	do {
		len = scall(read, &file,
			res.buf + res.len,
			(size_t) size - res.len);

		res.len += len;
	} while (len > 0);

	scall(destroy, &file);

	return res;
}
