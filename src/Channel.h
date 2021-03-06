#import "Kernel.h"

#define self Channel

#define exc(...)
exc(IsFolder)
exc(ReadingFailed)
exc(ReadingInterrupted)
exc(UnknownError)
exc(WritingFailed)
exc(WritingInterrupted)

set(ChannelId) {
	ChannelId_StdIn  = 0,
	ChannelId_StdOut = 1,
	ChannelId_StdErr = 2
};

class {
	/* File descriptor. */
	int id;

	/* File status flags. */
	int flags;
};

extern self* ref(StdIn);
extern self* ref(StdOut);
extern self* ref(StdErr);

rsdef(self, New, int id, int flags);
def(void, Destroy);
def(bool, IsBlocking);
def(void, SetBlocking, bool enable);
def(void, SetCloseOnExec, bool enable);
def(bool, IsReadable);
def(bool, IsWritable);
overload def(size_t, Read, void *buf, size_t len);
overload def(void, Read, String *res);
overload def(size_t, Write, void *buf, size_t len);

static alwaysInline def(int, GetId) {
	return this->id;
}

static alwaysInline def(int, GetFlags) {
	return this->flags;
}

static alwaysInline overload def(size_t, Write, RdString s) {
	return call(Write, s.buf, s.len);
}

static alwaysInline overload def(size_t, Write, char c) {
	return call(Write, &c, 1);
}

#undef self
