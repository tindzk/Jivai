#import <string.h> /* strlen() */
#import <alloca.h>

#import "Bit.h"
#import "Char.h"
#import "Types.h"
#import "Object.h"
#import "Macros.h"
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

record(RdString) {
	char   *buf;
	size_t len;
};

Instance(RdString);

typedef union OmniString {
	RdString rd;
	struct {
		char   *buf;
		size_t len;
	};
} OmniString;

Instance(OmniString);

typedef union self {
	RdString rd;
	struct {
		char   *buf;
		size_t len;
	};
} self;

Instance(self);

typedef union CarrierString {
	RdString rd;
	struct {
		char   *buf;
		size_t len;
		bool   omni;
	};
} CarrierString;

Instance(CarrierString);

record(FmtString) {
	RdString fmt;
	RdString *val;
};

#define $(s) ((RdString) {                       \
	.buf = (sizeof(s) == 1) ? NULL : (char *) s, \
	.len = sizeof(s) - 1                         \
})

#define $$(s) \
	(OmniString) $(s)

#undef self

#import "Array.h"
#import "Memory.h"

#define self String

Array(       String,        StringArray);
Array(     RdString,      RdStringArray);
Array(   OmniString,    OmniStringArray);
Array(CarrierString, CarrierStringArray);

sdef(size_t, GetSize, String s);
sdef(size_t, GetFree, String s);
def(void, Destroy);
def(void, Resize, size_t size);
def(void, Align, size_t size);
sdef(self, Clone, RdString s);
sdef(char, CharAt, RdString s, ssize_t offset);
overload sdef(RdString, Slice, RdString s, ssize_t offset, ssize_t length);
overload sdef(void, Crop, self *dest, ssize_t offset, ssize_t length);
def(void, Delete, ssize_t offset, ssize_t length);
overload sdef(void, Prepend, self *dest, RdString s);
overload sdef(void, Prepend, self *dest, char c);
def(void, Assign, String src);
def(void, Copy, RdString src);
overload sdef(void, Append, self *dest, RdString s);
overload sdef(void, Append, self *dest, char c);
overload sdef(void, Append, self *dest, FmtString s);
sdef(bool, RangeEquals, RdString s, ssize_t offset, RdString needle, ssize_t needleOffset);
def(void, ToLower);
def(void, ToUpper);
overload sdef(bool, Split, RdString s, char c, RdString *res);
overload sdef(bool, Split, RdString s, RdString needle, RdString *res);
overload sdef(RdStringArray *, Split, RdString s, char c);
sdef(bool, Parse, RdString pattern, RdString subject, ...);
overload sdef(ssize_t, Find, RdString s, ssize_t offset, ssize_t length, char c);
overload sdef(ssize_t, ReverseFind, RdString s, ssize_t offset, char c);
overload sdef(ssize_t, ReverseFind, RdString s, ssize_t offset, RdString needle);
overload sdef(ssize_t, Find, RdString s, ssize_t offset, ssize_t length, RdString needle);
overload sdef(RdString, Trim, RdString s, short type);
overload sdef(ssize_t, Between, RdString s, ssize_t offset, RdString left, RdString right, bool leftAligned, RdString *out);
sdef(RdString, Cut, RdString s, RdString left, RdString right);
def(bool, Filter, RdString s1, RdString s2);
def(bool, Outside, RdString left, RdString right);
overload sdef(self, Concat, RdString a, RdString b);
overload sdef(self, Concat, RdString s, char c);
overload sdef(bool, Replace, self *dest, ssize_t offset, RdString needle, RdString replacement);
overload sdef(bool, ReplaceAll, self *dest, ssize_t offset, RdString needle, RdString replacement);
sdef(short, CompareRight, RdString a, RdString b);
sdef(short, CompareLeft, RdString a, RdString b);
overload sdef(short, NaturalCompare, RdString a, RdString b, bool foldcase, bool skipSpaces, bool skipZeros);

#if String_FmtChecks
#define FmtString(_fmt, ...)  \
	(FmtString) {             \
		.fmt = _fmt,          \
		.val = (RdString[]) { \
			$(""),            \
			## __VA_ARGS__,   \
			(RdString) {      \
				.len = -1     \
			}                 \
		}                     \
	}
#else
#define FmtString(_fmt, ...)  \
	(FmtString) {             \
		.fmt = _fmt,          \
		.val = (RdString[]) { \
			__VA_ARGS__       \
		}                     \
	}
#endif

#define String_ToNul(s) \
	String_ToNulBuf(s, alloca((s).len + 1))

static alwaysInline OmniString RdString_Exalt(RdString s) {
	return (OmniString) s;
}

static inline sdef(self, New, size_t size) {
	if (size == 0) {
		return (self) { .buf = NULL };
	}

	return (self) {
		.buf = Memory_New(size)
	};
}

static alwaysInline overload rsdef(CarrierString, ToCarrier, String s) {
	return (CarrierString) {
		.buf = s.buf,
		.len = s.len,
		.omni = false
	};
}

static alwaysInline overload rsdef(CarrierString, ToCarrier, OmniString s) {
	return (CarrierString) {
		.buf = s.buf,
		.len = s.len,
		.omni = true
	};
}

static alwaysInline CarrierString CarrierString_New(void) {
	return (CarrierString) {
		.omni = true
	};
}

static inline void CarrierString_Destroy(CarrierStringInst $this) {
	if (!this->omni) {
		if (this->buf != NULL) {
			Memory_Destroy(this->buf);
		}

		this->omni = true;
	}

	this->buf = NULL;
	this->len = 0;
}

static alwaysInline void CarrierString_Assign(CarrierStringInst $this, CarrierString src) {
	CarrierString_Destroy(this);
	*this = src;
}

static inline String CarrierString_Flush(CarrierStringInst $this) {
	String res;

	 if (this->omni) {
		res = String_Clone(this->rd);
	 } else {
		res = (String) {
			.buf = this->buf,
			.len = this->len
		};
	 }

	this->buf = NULL;
	this->len = 0;
	this->omni = true;

	return res;
}

static alwaysInline sdef(self, Format, FmtString fmt) {
	self res = scall(New, 0);
	scall(Append, &res, fmt);
	return res;
}

#define String_Format(...) \
	String_Format(FmtString(__VA_ARGS__))

static alwaysInline sdef(RdString, FromNul, char *s) {
	return (RdString) {
		.buf = s,
		.len = (s != NULL)
			? strlen(s)
			: 0
	};
}

static inline sdef(char *, ToNulBuf, RdString s, char *buf) {
	if (s.len > 0) {
		Memory_Copy(buf, s.buf, s.len);
	}

	buf[s.len] = '\0';

	return buf;
}

static alwaysInline sdef(char *, ToNulHeap, RdString s) {
	return scall(ToNulBuf, s, Memory_New(s.len + 1));
}

static inline overload sdef(RdString, Slice, RdString s, ssize_t offset) {
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

static alwaysInline sdef(bool, Equals, RdString s, RdString needle) {
	return s.len == needle.len && Memory_Equals(s.buf, needle.buf, s.len);
}

static alwaysInline sdef(bool, BeginsWith, RdString s, RdString needle) {
	return scall(RangeEquals, s, 0, needle, 0);
}

static alwaysInline sdef(bool, EndsWith, RdString s, RdString needle) {
	return scall(RangeEquals, s, s.len - needle.len, needle, 0);
}

static alwaysInline overload sdef(ssize_t, ReverseFind, RdString s, char c) {
	return scall(ReverseFind, s, s.len - 1, c);
}

static alwaysInline overload sdef(ssize_t, ReverseFind, RdString s, RdString needle) {
	return scall(ReverseFind, s, s.len - 1, needle);
}

static alwaysInline overload sdef(ssize_t, Find, RdString s, RdString needle) {
	return scall(Find, s, 0, s.len, needle);
}

static inline overload sdef(ssize_t, Find, RdString s, ssize_t offset, RdString needle) {
	if (offset < 0) {
		offset += s.len;
	}

	return scall(Find, s, offset, s.len - offset, needle);
}

static alwaysInline overload sdef(ssize_t, Find, RdString s, char c) {
	return scall(Find, s, 0, s.len, c);
}

static inline overload sdef(ssize_t, Find, RdString s, ssize_t offset, char c) {
	if (offset < 0) {
		offset += s.len;
	}

	return scall(Find, s, offset, s.len - offset, c);
}

static alwaysInline overload sdef(bool, Contains, RdString s, RdString needle) {
	return scall(Find, s, 0, s.len, needle) != ref(NotFound);
}

static alwaysInline overload sdef(bool, Contains, RdString s, char needle) {
	return scall(Find, s, 0, s.len, needle) != ref(NotFound);
}

static alwaysInline overload sdef(RdString, Trim, RdString s) {
	return scall(Trim, s,
		ref(TrimLeft) |
		ref(TrimRight));
}

static alwaysInline overload sdef(ssize_t, Between, RdString s, RdString left, RdString right, RdString *out) {
	return scall(Between, s, 0, left, right, true, out);
}

static alwaysInline overload sdef(ssize_t, Between, RdString s, ssize_t offset, RdString left, RdString right, RdString *out) {
	return scall(Between, s, offset, left, right, true, out);
}

static alwaysInline overload sdef(RdString, Between, RdString s, ssize_t offset, RdString left, RdString right, bool leftAligned) {
	RdString out = $("");
	scall(Between, s, offset, left, right, leftAligned, &out);
	return out;
}

static alwaysInline overload sdef(RdString, Between, RdString s, ssize_t offset, RdString left, RdString right) {
	RdString out = $("");
	scall(Between, s, offset, left, right, &out);
	return out;
}

static alwaysInline overload sdef(RdString, Between, RdString s, RdString left, RdString right, bool leftAligned) {
	RdString out = $("");
	scall(Between, s, 0, left, right, leftAligned, &out);
	return out;
}

static alwaysInline overload sdef(RdString, Between, RdString s, RdString left, RdString right) {
	RdString out = $("");
	scall(Between, s, 0, left, right, true, &out);
	return out;
}

static alwaysInline overload sdef(bool, Replace, self *dest, RdString needle, RdString replacement) {
	return scall(Replace, dest, 0, needle, replacement);
}

static alwaysInline overload sdef(self, Replace, self s, RdString needle, RdString replacement) {
	self tmp = s;
	scall(Replace, &tmp, 0, needle, replacement);
	return tmp;
}

static alwaysInline overload sdef(bool, ReplaceAll, self *dest, RdString needle, RdString replacement) {
	return scall(ReplaceAll, dest, 0, needle, replacement);
}

static alwaysInline overload sdef(self, ReplaceAll, RdString s, RdString needle, RdString replacement) {
	self tmp = scall(Clone, s);
	scall(ReplaceAll, &tmp, 0, needle, replacement);
	return tmp;
}

static alwaysInline overload sdef(short, NaturalCompare, RdString a, RdString b) {
	return scall(NaturalCompare, a, b, true, true, true);
}

#undef self

#import "Buffer.h"

static alwaysInline RdBuffer String_GetRdBuffer(RdString str) {
	return (RdBuffer) {
		.ptr = str.buf,
		.len = str.len
	};
}

static alwaysInline WrBuffer String_GetWrBuffer(String str) {
	return (WrBuffer) {
		.ptr  = str.buf,
		.size = String_GetSize(str)
	};
}

#define self RdStringArray
def(ssize_t, Find, RdString needle);
def(String, Join, RdString separator);
def(bool, Contains, RdString needle);
#undef self

#define self StringArray
def(ssize_t, Find, RdString needle);
def(bool, Contains, RdString needle);
def(void, Destroy);
#undef self

#define self CarrierStringArray
def(void, Destroy);
#undef self
