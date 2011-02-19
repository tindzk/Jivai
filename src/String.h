#import "Bit.h"
#import "Types.h"
#import "BitMask.h"
#import "Compiler.h"

#define self String

#ifndef String_SmartAlign
#define String_SmartAlign 1
#endif

#ifndef String_FmtChecks
#define String_FmtChecks 1
#endif

// @exc DoubleFree
// @exc IsReadOnly
// @exc BufferOverflow
// @exc ElementMismatch

enum {
	ref(TrimLeft)  = Bit(0),
	ref(TrimRight) = Bit(1),
	ref(NotFound)  = -1
};

class {
	/* Character buffer including offset. */
	char *buf;
	size_t ofs;
	size_t len;
	bool readonly;
};

record(FmtString) {
	String fmt;
	String *val;
};

#undef self

#import "Char.h"
#import "Array.h"
#import "Arena.h"
#import "Memory.h"
#import "Exception.h"

#define self String

Array(self, StringArray);

def(size_t, GetSize);
def(size_t, GetFree);
def(void, Free);
def(void, Destroy);
def(void, Resize, size_t length);
def(void, Align, size_t length);
sdef(self, Clone, self s);
sdef(char, CharAt, self s, ssize_t offset);
overload sdef(self, Slice, self s, ssize_t offset, ssize_t length);
overload sdef(void, Crop, self *dest, ssize_t offset, ssize_t length);
overload sdef(void, FastCrop, self *dest, ssize_t offset, ssize_t length);
def(void, Shift);
def(void, Delete, ssize_t offset, ssize_t length);
overload sdef(void, Prepend, self *dest, self s);
overload sdef(void, Prepend, self *dest, char c);
def(void, Copy, self src);
overload sdef(void, Append, self *dest, self s);
overload sdef(void, Append, self *dest, char c);
overload sdef(void, Append, self *dest, FmtString s);
sdef(bool, RangeEquals, self s, ssize_t offset, self needle, ssize_t needleOffset);
def(void, ToLower);
def(void, ToUpper);
overload sdef(bool, Split, self s, char c, String *res);
overload sdef(StringArray *, Split, self s, char c);
overload sdef(ssize_t, Find, self s, ssize_t offset, ssize_t length, char c);
overload sdef(ssize_t, ReverseFind, self s, ssize_t offset, char c);
overload sdef(ssize_t, ReverseFind, self s, ssize_t offset, self needle);
overload sdef(ssize_t, Find, self s, ssize_t offset, ssize_t length, self needle);
overload sdef(self, Trim, self s, short type);
sdef(self, Format, self fmt, ...);
overload sdef(ssize_t, Between, self s, ssize_t offset, self left, self right, bool leftAligned, self *out);
sdef(self, Cut, self s, self left, self right);
def(bool, Filter, self s1, self s2);
def(bool, Outside, self left, self right);
overload sdef(self, Concat, self a, self b);
overload sdef(self, Concat, self s, char c);
overload sdef(bool, Replace, self *dest, ssize_t offset, self needle, self replacement);
overload sdef(bool, ReplaceAll, self *dest, ssize_t offset, self needle, self replacement);
def(self, Consume, size_t n);
sdef(short, CompareRight, self a, self b);
sdef(short, CompareLeft, self a, self b);
overload sdef(short, NaturalCompare, self a, self b, bool foldcase, bool skipSpaces, bool skipZeros);

#define String_Print(s) \
	File_Write(File_StdOut, s)

#if String_FmtChecks
#define FmtString(_fmt, ...) \
	(FmtString) {            \
		.fmt = _fmt,         \
		.val = (String[]) {  \
			$(""),           \
			## __VA_ARGS__,  \
			(String) {       \
				.len = -1    \
			}                \
		}                    \
	}
#else
#define FmtString(_fmt, ...) \
	(FmtString) {            \
		.fmt = _fmt,         \
		.val = (String[]) {  \
			__VA_ARGS__      \
		}                    \
	}
#endif

#define $(s) ((String) {                         \
	.buf = (sizeof(s) == 1) ? NULL : (char *) s, \
	.len = sizeof(s) - 1,                        \
	.readonly = true                             \
})

#define String_ToNul(s) \
	String_ToNulBuf(s, alloca((s).len + 1))

static inline sdef(self, New, size_t size) {
	if (size == 0) {
		return (self) { .readonly = true };
	}

	return (self) {
		.buf = Arena_Alloc(Arena_GetInstance(), size)
	};
}

static inline sdef(self, FromNul, char *s) {
	return (self) {
		.buf = s,
		.len = (s != NULL)
			? strlen(s)
			: 0
	};
}

static inline sdef(char *, ToNulBuf, self s, char *buf) {
	if (s.len > 0) {
		Memory_Copy(buf, s.buf, s.len);
	}

	buf[s.len] = '\0';

	return buf;
}

static inline sdef(char *, ToNulHeap, self s) {
	return scall(ToNulBuf, s,
		Arena_Alloc(Arena_GetInstance(), s.len + 1));
}

static inline sdef(self, Disown, self s) {
	s.readonly = true;
	return s;
}

static inline overload sdef(self, Slice, self s, ssize_t offset) {
	if (offset < 0) {
		offset += s.len;
	}

	return scall(Slice, s, offset, s.len - offset);
}

static inline overload sdef(void, Crop, self *dest, ssize_t offset) {
	if (offset < 0) {
		offset += dest->len;
	}

	scall(Crop, dest, offset, dest->len - offset);
}

static inline overload sdef(void, FastCrop, self *dest, ssize_t offset) {
	if (offset < 0) {
		offset += dest->len;
	}

	scall(FastCrop, dest, offset, dest->len - offset);
}

static inline sdef(bool, Equals, self s, self needle) {
	return s.len == needle.len && Memory_Equals(s.buf, needle.buf, s.len);
}

static inline sdef(bool, BeginsWith, self s, self needle) {
	return scall(RangeEquals, s, 0, needle, 0);
}

static inline sdef(bool, EndsWith, self s, self needle) {
	return scall(RangeEquals, s, s.len - needle.len, needle, 0);
}

static inline overload sdef(ssize_t, ReverseFind, self s, char c) {
	return scall(ReverseFind, s, s.len - 1, c);
}

static inline overload sdef(ssize_t, ReverseFind, self s, self needle) {
	return scall(ReverseFind, s, s.len - 1, needle);
}

static inline overload sdef(ssize_t, Find, self s, self needle) {
	return scall(Find, s, 0, s.len, needle);
}

static inline overload sdef(ssize_t, Find, self s, ssize_t offset, self needle) {
	if (offset < 0) {
		offset += s.len;
	}

	return scall(Find, s, offset, s.len - offset, needle);
}

static inline overload sdef(ssize_t, Find, self s, char c) {
	return scall(Find, s, 0, s.len, c);
}

static inline overload sdef(ssize_t, Find, self s, ssize_t offset, char c) {
	if (offset < 0) {
		offset += s.len;
	}

	return scall(Find, s, offset, s.len - offset, c);
}

static inline overload sdef(bool, Contains, self s, self needle) {
	return scall(Find, s, 0, s.len, needle) != ref(NotFound);
}

static inline overload sdef(bool, Contains, self s, char needle) {
	return scall(Find, s, 0, s.len, needle) != ref(NotFound);
}

static inline overload sdef(self, Trim, self s) {
	return scall(Trim, s,
		ref(TrimLeft) |
		ref(TrimRight));
}

static inline overload sdef(ssize_t, Between, self s, self left, self right, self *out) {
	return scall(Between, s, 0, left, right, true, out);
}

static inline overload sdef(ssize_t, Between, self s, ssize_t offset, self left, self right, self *out) {
	return scall(Between, s, offset, left, right, true, out);
}

static inline overload sdef(self, Between, self s, ssize_t offset, self left, self right, bool leftAligned) {
	self out = $("");
	scall(Between, s, offset, left, right, leftAligned, &out);
	return out;
}

static inline overload sdef(self, Between, self s, ssize_t offset, self left, self right) {
	self out = $("");
	scall(Between, s, offset, left, right, &out);
	return out;
}

static inline overload sdef(self, Between, self s, self left, self right, bool leftAligned) {
	self out = $("");
	scall(Between, s, 0, left, right, leftAligned, &out);
	return out;
}

static inline overload sdef(self, Between, self s, self left, self right) {
	self out = $("");
	scall(Between, s, 0, left, right, true, &out);
	return out;
}

static inline overload sdef(bool, Replace, self *dest, self needle, self replacement) {
	return scall(Replace, dest, 0, needle, replacement);
}

static inline overload sdef(self, Replace, self s, self needle, self replacement) {
	self tmp = s;
	scall(Replace, &tmp, 0, needle, replacement);
	return tmp;
}

static inline overload sdef(bool, ReplaceAll, self *dest, self needle, self replacement) {
	return scall(ReplaceAll, dest, 0, needle, replacement);
}

static inline overload sdef(self, ReplaceAll, self s, self needle, self replacement) {
	self tmp = scall(Clone, s);
	scall(ReplaceAll, &tmp, 0, needle, replacement);
	return tmp;
}

static inline overload sdef(short, NaturalCompare, self a, self b) {
	return scall(NaturalCompare, a, b, true, true, true);
}

#undef self

#define self StringArray

def(ssize_t, Find, String needle);
def(String, Join, String separator);
def(bool, Contains, String needle);
def(void, Destroy);

#undef self
