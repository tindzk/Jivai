#import "Memory.h"

#define self Buffer

/* Read-only buffer.
 * `.len' is the number of occupied bytes.
 */
record(RdBuffer) {
	size_t len;
	void   *ptr;
};

/* Write-only buffer.
 * Don't read from WrBuffer because `.size' corresponds to the allocated size,
 * not the actual number of occupied bytes.
 */
typedef union WrBuffer {
	RdBuffer rd;

	struct {
		size_t size;
		void   *ptr;
	};
} WrBuffer;

/* Can be written to and read from. */
typedef union self {
	RdBuffer rd;

	/* This interprets `.len' as the occupied size! If this isn't desired, use
	 * AsWrBuffer().
	 */
	WrBuffer wr;

	struct {
		size_t len;
		void   *ptr;
	};
} self;

Instance(self);

rsdef(self, New, size_t size);
def(void, Destroy);
rdef(size_t, GetSize);
overload rdef(self, Clone);
overload rsdef(self, Clone, RdBuffer buf);
rsdef(bool, IsValid, RdBuffer buf);
def(void, Resize, size_t size);
def(void, Align, size_t length);
sdef(void, Move, WrBuffer dest, RdBuffer src);
sdef(void, Copy, WrBuffer dest, RdBuffer src);
overload rsdef(bool, Equals, RdBuffer buf1, RdBuffer buf2);
rdef(WrBuffer, AsWrBuffer);

static alwaysInline rsdef(WrBuffer, ForChar, char *c) {
	return (WrBuffer) {
		.ptr  = c,
		.size = 1
	};
}

#undef self
