#import "Bit.h"
#import "Types.h"
#import "BitMask.h"
#import "Compiler.h"

#define self String

#ifndef String_SmartAlign
#define String_SmartAlign 1
#endif

class {
	size_t len;
	size_t size;
	char *buf;
	bool mutable;
};

enum {
	ref(TrimLeft)  = Bit(0),
	ref(TrimRight) = Bit(1),
	ref(NotFound)  = -1
};

#undef self

#import "Char.h"
#import "Array.h"
#import "Kernel.h"
#import "Memory.h"
#import "Exception.h"

// @exc NotMutable
// @exc BufferOverflow

#define self String

Array(self, StringArray);

self HeapString(size_t len);
self BufString(char *buf, size_t len);
def(void, Destroy);
sdef(self, FromNul, char *s);
sdef(char *, ToNulBuf, self s, char *buf);
sdef(char *, ToNulHeap, self s);
sdef(self, Disown, self s);
def(void, Resize, size_t length);
def(void, Align, size_t length);
sdef(void, Copy, self *dest, self src);
sdef(self, Clone, self s);
sdef(char *, CloneBuf, self s, char *buf);
sdef(char, CharAt, self s, ssize_t offset);
overload sdef(self, Slice, self s, ssize_t offset, ssize_t length);
overload sdef(self, Slice, self s, ssize_t offset);
overload sdef(void, Crop, self *dest, ssize_t offset, ssize_t length);
overload sdef(void, Crop, self *dest, ssize_t offset);
def(void, Delete, ssize_t offset, ssize_t length);
overload sdef(void, Prepend, self *dest, self s);
overload sdef(void, Prepend, self *dest, char c);
overload sdef(void, Append, self *dest, self s);
overload sdef(void, Append, self *dest, char c);
sdef(self, Join, self *first, ...);
sdef(bool, Equals, self s, self needle);
sdef(bool, RangeEquals, self s, ssize_t offset, self needle, ssize_t needleOffset);
sdef(bool, BeginsWith, self s, self needle);
sdef(bool, EndsWith, self s, self needle);
def(void, ToLower);
def(void, ToUpper);
overload sdef(StringArray *, Split, self s, size_t offset, char c);
overload sdef(StringArray *, Split, self s, char c);
overload sdef(ssize_t, Find, self s, ssize_t offset, ssize_t length, char c);
overload sdef(ssize_t, ReverseFind, self s, ssize_t offset, char c);
overload sdef(ssize_t, ReverseFind, self s, char c);
overload sdef(ssize_t, ReverseFind, self s, ssize_t offset, self needle);
overload sdef(ssize_t, ReverseFind, self s, self needle);
overload sdef(ssize_t, Find, self s, ssize_t offset, ssize_t length, self needle);
overload sdef(ssize_t, Find, self s, self needle);
overload sdef(ssize_t, Find, self s, ssize_t offset, self needle);
overload sdef(ssize_t, Find, self s, char c);
overload sdef(ssize_t, Find, self s, ssize_t offset, char c);
overload sdef(bool, Contains, self s, self needle);
overload sdef(bool, Contains, self s, char needle);
overload sdef(void, Trim, self *dest, short type);
overload sdef(void, Trim, self *dest);
overload sdef(self, Trim, self s, short type);
overload sdef(self, Trim, self s);
sdef(self, Format, self fmt, ...);
overload sdef(ssize_t, Between, self s, ssize_t offset, self left, self right, bool leftAligned, self *out);
overload sdef(ssize_t, Between, self s, self left, self right, self *out);
overload sdef(ssize_t, Between, self s, ssize_t offset, self left, self right, self *out);
overload sdef(self, Between, self s, ssize_t offset, self left, self right, bool leftAligned);
overload sdef(self, Between, self s, ssize_t offset, self left, self right);
overload sdef(self, Between, self s, self left, self right, bool leftAligned);
overload sdef(self, Between, self s, self left, self right);
sdef(self, Cut, self s, self left, self right);
def(bool, Filter, self s1, self s2);
def(bool, Outside, self left, self right);
overload sdef(self, Concat, self a, self b);
overload sdef(self, Concat, self s, char c);
overload sdef(bool, Replace, self *dest, ssize_t offset, self needle, self replacement);
overload sdef(bool, Replace, self *dest, self needle, self replacement);
overload sdef(self, Replace, self s, self needle, self replacement);
overload sdef(bool, ReplaceAll, self *dest, ssize_t offset, self needle, self replacement);
overload sdef(bool, ReplaceAll, self *dest, self needle, self replacement);
overload sdef(self, ReplaceAll, self s, self needle, self replacement);
def(self, Consume, size_t n);
overload sdef(void, Print, self s, bool err);
overload sdef(void, Print, self s);
sdef(short, CompareRight, self a, self b);
sdef(short, CompareLeft, self a, self b);
overload sdef(short, NaturalCompare, self a, self b, bool foldcase, bool skipSpaces, bool skipZeros);
overload sdef(short, NaturalCompare, self a, self b);

#undef self

#define self StringArray

def(ssize_t, Find, String needle);
def(String, Join, String separator);
def(bool, Contains, String needle);
def(void, Destroy);
def(void, ToHeap);

#define $(s) \
	(String) { sizeof(s) - 1, sizeof(s) - 1, (char *) s, false }

#define NullString \
	(String) { 0, 0, NULL, false }

#define StackString(len) \
	(String) { 0, len, ((len) > 0) ? alloca((len)) : NULL, false }

#define String_StackClone(s) \
	(String) { (s).len, (s).len, String_CloneBuf(s, alloca((s).len)), false }

#define String_ToNul(s) \
	String_ToNulBuf(s, alloca((s).len + 1))

#define String_FmtPrint(...)            \
	do {                                \
		String __tmp =                  \
			String_Format(__VA_ARGS__); \
		String_Print(__tmp);            \
		String_Destroy(&__tmp);         \
	} while(0)

#define String_FmtAppend(this, ...)     \
	do {                                \
		String __tmp =                  \
			String_Format(__VA_ARGS__); \
		String_Append(this, __tmp);     \
		String_Destroy(&__tmp);         \
	} while(0)

#undef self
