#import "Channel.h"

#define self Channel

static self stdIn = {
	.id    = FileNo_StdIn,
	.flags = FileStatus_ReadOnly
};

static self stdOut = {
	.id    = FileNo_StdOut,
	.flags = FileStatus_WriteOnly
};

static self stdErr = {
	.id    = FileNo_StdErr,
	.flags = FileStatus_WriteOnly
};

self* ref(StdIn)  = &stdIn;
self* ref(StdOut) = &stdOut;
self* ref(StdErr) = &stdErr;

rsdef(self, New, int id, int flags) {
	return (self) {
		.id = id,
		.flags = flags
	};
}

def(void, Destroy) {
	if (Kernel_close(this->id) == -1) {
		throw(UnknownError);
	}
}

def(bool, IsBlocking) {
	return !BitMask_Has(this->flags, FileStatus_NonBlock);
}

def(void, SetBlocking, bool enable) {
	int old = this->flags;

	if (enable) {
		BitMask_Clear(this->flags, FileStatus_NonBlock);
	} else {
		BitMask_Set(this->flags, FileStatus_NonBlock);
	}

	if (old == this->flags) {
		return;
	}

	if (Kernel_fcntl(this->id, FcntlMode_SetStatus, this->flags) == -1) {
		throw(UnknownError);
	}
}

/* As CloseOnExec is the only supported file descriptor flag, we don't need to
 * store the descriptor flags in the object.
 */
def(void, SetCloseOnExec, bool enable) {
	int flag = 0;

	if (enable) {
		flag = FileDescriptorFlags_CloseOnExec;
	}

	if (Kernel_fcntl(this->id, FcntlMode_SetDescriptorFlags, flag) == -1) {
		throw(UnknownError);
	}
}

def(bool, IsReadable) {
	switch (this->flags & 3) {
		case FileStatus_ReadWrite:
		case FileStatus_ReadOnly:
			return true;

		default:
			return false;
	}
}

def(bool, IsWritable) {
	switch (this->flags & 3) {
		case FileStatus_ReadWrite:
		case FileStatus_WriteOnly:
			return true;

		default:
			return false;
	}
}

overload def(size_t, Read, void *buf, size_t len) {
	assert(call(IsReadable));

	errno = 0;

	ssize_t res;

	if ((res = Kernel_read(this->id, buf, len)) == -1) {
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

overload def(void, Read, String *res) {
	res->len = call(Read, res->buf, String_GetSize(*res));
}

overload def(size_t, Write, void *buf, size_t len) {
	assert(call(IsWritable));

	errno = 0;

	ssize_t res;

	if ((res = Kernel_write(this->id, buf, len)) == -1) {
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
