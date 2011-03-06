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
// @exc BufferOverflow
// @exc ElementMismatch

enum {
	ref(TrimLeft)  = Bit(0),
	ref(TrimRight) = Bit(1),
	ref(NotFound)  = -1
};

record(ProtString) {
	char   *buf;
	size_t len;
	size_t ofs;
};

Instance(ProtString);

typedef union CarrierString {
	ProtString prot;
	struct {
		char   *buf;
		size_t len;
		size_t ofs;
		bool   isProt;
	};
} CarrierString;

Instance(CarrierString);

typedef union self {
	ProtString prot;
	struct {
		char   *buf;
		size_t len;
		size_t ofs;
	};
} self;

Instance(self);
InstanceMethods(self);

record(FmtString) {
	ProtString fmt;
	ProtString *val;
};

#define $(s) ((ProtString) {                     \
	.buf = (sizeof(s) == 1) ? NULL : (char *) s, \
	.len = sizeof(s) - 1                         \
})

#undef self

#import "Char.h"
#import "Pool.h"
#import "Array.h"
#import "Memory.h"
#import "Exception.h"

#define self String

Array(       String,        StringArray);
Array(   ProtString,    ProtStringArray);
Array(CarrierString, CarrierStringArray);

sdef(size_t, GetSize, String s);
sdef(size_t, GetFree, String s);
def(void, Destroy);
def(void, Resize, size_t length);
def(void, Align, size_t length);
sdef(self, Clone, ProtString s);
sdef(char, CharAt, ProtString s, ssize_t offset);
overload sdef(ProtString, Slice, ProtString s, ssize_t offset, ssize_t length);
overload sdef(void, Crop, self *dest, ssize_t offset, ssize_t length);
overload sdef(void, FastCrop, self *dest, ssize_t offset, ssize_t length);
def(void, Shift);
def(void, Delete, ssize_t offset, ssize_t length);
overload sdef(void, Prepend, self *dest, ProtString s);
overload sdef(void, Prepend, self *dest, char c);
def(void, Assign, String src);
def(void, Copy, ProtString src);
overload sdef(void, Append, self *dest, ProtString s);
overload sdef(void, Append, self *dest, char c);
overload sdef(void, Append, self *dest, FmtString s);
sdef(bool, RangeEquals, ProtString s, ssize_t offset, ProtString needle, ssize_t needleOffset);
def(void, ToLower);
def(void, ToUpper);
overload sdef(bool, Split, ProtString s, char c, ProtString *res);
overload sdef(ProtStringArray *, Split, ProtString s, char c);
overload sdef(ssize_t, Find, ProtString s, ssize_t offset, ssize_t length, char c);
overload sdef(ssize_t, ReverseFind, ProtString s, ssize_t offset, char c);
overload sdef(ssize_t, ReverseFind, ProtString s, ssize_t offset, ProtString needle);
overload sdef(ssize_t, Find, ProtString s, ssize_t offset, ssize_t length, ProtString needle);
overload sdef(ProtString, Trim, ProtString s, short type);
overload sdef(ssize_t, Between, ProtString s, ssize_t offset, ProtString left, ProtString right, bool leftAligned, ProtString *out);
sdef(ProtString, Cut, ProtString s, ProtString left, ProtString right);
def(bool, Filter, ProtString s1, ProtString s2);
def(bool, Outside, ProtString left, ProtString right);
overload sdef(self, Concat, ProtString a, ProtString b);
overload sdef(self, Concat, ProtString s, char c);
overload sdef(bool, Replace, self *dest, ssize_t offset, ProtString needle, ProtString replacement);
overload sdef(bool, ReplaceAll, self *dest, ssize_t offset, ProtString needle, ProtString replacement);
sdef(short, CompareRight, ProtString a, ProtString b);
sdef(short, CompareLeft, ProtString a, ProtString b);
overload sdef(short, NaturalCompare, ProtString a, ProtString b, bool foldcase, bool skipSpaces, bool skipZeros);

#define String_Print(s) \
	File_Write(File_StdOut, s)

#if String_FmtChecks
#define FmtString(_fmt, ...)    \
	(FmtString) {               \
		.fmt = _fmt,            \
		.val = (ProtString[]) { \
			$(""),              \
			## __VA_ARGS__,     \
			(ProtString) {      \
				.len = -1       \
			}                   \
		}                       \
	}
#else
#define FmtString(_fmt, ...)    \
	(FmtString) {               \
		.fmt = _fmt,            \
		.val = (ProtString[]) { \
			__VA_ARGS__         \
		}                       \
	}
#endif

#define String_ToNul(s) \
	String_ToNulBuf(s, alloca((s).len + 1))

static inline sdef(self, New, size_t size) {
	if (size == 0) {
		return (self) { .buf = NULL };
	}

	return (self) {
		.buf = Pool_Alloc(Pool_GetInstance(), size)
	};
}

static inline overload rsdef(CarrierString, ToCarrier, String s) {
	return (CarrierString) {
		.buf = s.buf,
		.len = s.len,
		.ofs = s.ofs,
		.isProt = false
	};
}

static inline overload rsdef(CarrierString, ToCarrier, ProtString s) {
	return (CarrierString) {
		.buf = s.buf,
		.len = s.len,
		.ofs = s.ofs,
		.isProt = true
	};
}

static inline CarrierString CarrierString_New(void) {
	return (CarrierString) {
		.isProt = true
	};
}

static inline void CarrierString_Destroy(CarrierStringInstance $this) {
	if (!this->isProt) {
		if (this->buf != NULL) {
			Pool_Free(Pool_GetInstance(), this->buf - this->ofs);
		}

		this->isProt = true;
	}

	this->buf = NULL;
	this->len = 0;
}

static inline void CarrierString_Assign(CarrierStringInstance $this, CarrierString src) {
	CarrierString_Destroy(this);
	*this = src;
}

static inline String CarrierString_Flush(CarrierStringInstance $this) {
	String res;

	 if (this->isProt) {
		res = String_Clone(this->prot);
	 } else {
		res = (String) {
			.buf = this->buf,
			.len = this->len,
			.ofs = this->ofs
		};
	 }

	this->buf = NULL;
	this->len = 0;
	this->isProt = true;

	return res;
}

static inline sdef(self, Format, FmtString fmt) {
	self res = scall(New, 0);
	scall(Append, &res, fmt);
	return res;
}

#define String_Format(...) \
	String_Format(FmtString(__VA_ARGS__))

static inline sdef(ProtString, FromNul, char *s) {
	return (ProtString) {
		.buf = s,
		.len = (s != NULL)
			? strlen(s)
			: 0
	};
}

static inline sdef(char *, ToNulBuf, ProtString s, char *buf) {
	if (s.len > 0) {
		Memory_Copy(buf, s.buf, s.len);
	}

	buf[s.len] = '\0';

	return buf;
}

static inline sdef(char *, ToNulHeap, ProtString s) {
	return scall(ToNulBuf, s,
		Pool_Alloc(Pool_GetInstance(), s.len + 1));
}

static inline overload sdef(ProtString, Slice, ProtString s, ssize_t offset) {
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

static inline sdef(bool, Equals, ProtString s, ProtString needle) {
	return s.len == needle.len && Memory_Equals(s.buf, needle.buf, s.len);
}

static inline sdef(bool, BeginsWith, ProtString s, ProtString needle) {
	return scall(RangeEquals, s, 0, needle, 0);
}

static inline sdef(bool, EndsWith, ProtString s, ProtString needle) {
	return scall(RangeEquals, s, s.len - needle.len, needle, 0);
}

static inline overload sdef(ssize_t, ReverseFind, ProtString s, char c) {
	return scall(ReverseFind, s, s.len - 1, c);
}

static inline overload sdef(ssize_t, ReverseFind, ProtString s, ProtString needle) {
	return scall(ReverseFind, s, s.len - 1, needle);
}

static inline overload sdef(ssize_t, Find, ProtString s, ProtString needle) {
	return scall(Find, s, 0, s.len, needle);
}

static inline overload sdef(ssize_t, Find, ProtString s, ssize_t offset, ProtString needle) {
	if (offset < 0) {
		offset += s.len;
	}

	return scall(Find, s, offset, s.len - offset, needle);
}

static inline overload sdef(ssize_t, Find, ProtString s, char c) {
	return scall(Find, s, 0, s.len, c);
}

static inline overload sdef(ssize_t, Find, ProtString s, ssize_t offset, char c) {
	if (offset < 0) {
		offset += s.len;
	}

	return scall(Find, s, offset, s.len - offset, c);
}

static inline overload sdef(bool, Contains, ProtString s, ProtString needle) {
	return scall(Find, s, 0, s.len, needle) != ref(NotFound);
}

static inline overload sdef(bool, Contains, ProtString s, char needle) {
	return scall(Find, s, 0, s.len, needle) != ref(NotFound);
}

static inline overload sdef(ProtString, Trim, ProtString s) {
	return scall(Trim, s,
		ref(TrimLeft) |
		ref(TrimRight));
}

static inline overload sdef(ssize_t, Between, ProtString s, ProtString left, ProtString right, ProtString *out) {
	return scall(Between, s, 0, left, right, true, out);
}

static inline overload sdef(ssize_t, Between, ProtString s, ssize_t offset, ProtString left, ProtString right, ProtString *out) {
	return scall(Between, s, offset, left, right, true, out);
}

static inline overload sdef(ProtString, Between, ProtString s, ssize_t offset, ProtString left, ProtString right, bool leftAligned) {
	ProtString out = $("");
	scall(Between, s, offset, left, right, leftAligned, &out);
	return out;
}

static inline overload sdef(ProtString, Between, ProtString s, ssize_t offset, ProtString left, ProtString right) {
	ProtString out = $("");
	scall(Between, s, offset, left, right, &out);
	return out;
}

static inline overload sdef(ProtString, Between, ProtString s, ProtString left, ProtString right, bool leftAligned) {
	ProtString out = $("");
	scall(Between, s, 0, left, right, leftAligned, &out);
	return out;
}

static inline overload sdef(ProtString, Between, ProtString s, ProtString left, ProtString right) {
	ProtString out = $("");
	scall(Between, s, 0, left, right, true, &out);
	return out;
}

static inline overload sdef(bool, Replace, self *dest, ProtString needle, ProtString replacement) {
	return scall(Replace, dest, 0, needle, replacement);
}

static inline overload sdef(self, Replace, self s, ProtString needle, ProtString replacement) {
	self tmp = s;
	scall(Replace, &tmp, 0, needle, replacement);
	return tmp;
}

static inline overload sdef(bool, ReplaceAll, self *dest, ProtString needle, ProtString replacement) {
	return scall(ReplaceAll, dest, 0, needle, replacement);
}

static inline overload sdef(self, ReplaceAll, ProtString s, ProtString needle, ProtString replacement) {
	self tmp = scall(Clone, s);
	scall(ReplaceAll, &tmp, 0, needle, replacement);
	return tmp;
}

static inline overload sdef(short, NaturalCompare, ProtString a, ProtString b) {
	return scall(NaturalCompare, a, b, true, true, true);
}

#undef self

#define self ProtStringArray
def(ssize_t, Find, ProtString needle);
def(String, Join, ProtString separator);
def(bool, Contains, ProtString needle);
#undef self

#define self StringArray
def(void, Destroy);
#undef self
