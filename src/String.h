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
// @exc IsInherited
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
	size_t len;

	/* If set, one of these will become the new owner of the buffer upon
	 * destruction.
	 */
	String *prev;
	String *next;

	bool inherited;
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

sdef(self, New, size_t size);
def(size_t, GetSize);
def(size_t, GetFree);
def(void, Free);
def(void, Unlink);
def(void, Decouple);
sdef(void, Insert, String *src, String *res);
def(void, Destroy);
sdef(self, FromNul, char *s);
sdef(char *, ToNulBuf, self s, char *buf);
sdef(char *, ToNulHeap, self s);
sdef(self, Disown, self s);
def(void, Resize, size_t length);
def(void, Align, size_t length);
def(void, Assign, self *src);
def(void, Clear);
sdef(self, Clone, self s);
sdef(self *, SafeClone, String *src);
sdef(char, CharAt, self s, ssize_t offset);
overload sdef(self, Slice, self s, ssize_t offset, ssize_t length);
overload sdef(self, Slice, self s, ssize_t offset);
overload sdef(String *, SafeSlice, self *s, ssize_t offset, ssize_t length);
overload sdef(String *, SafeSlice, self *s, ssize_t offset);
overload sdef(void, Crop, self *dest, ssize_t offset, ssize_t length);
overload sdef(void, Crop, self *dest, ssize_t offset);
overload sdef(void, FastCrop, self *dest, ssize_t offset, ssize_t length);
overload sdef(void, FastCrop, self *dest, ssize_t offset);
def(void, Shift);
def(void, Delete, ssize_t offset, ssize_t length);
overload sdef(void, Prepend, self *dest, self s);
overload sdef(void, Prepend, self *dest, char c);
def(void, Copy, self src);
overload sdef(void, Append, self *dest, self s);
overload sdef(void, Append, self *dest, char c);
overload sdef(void, Append, self *dest, FmtString s);
sdef(bool, Equals, self s, self needle);
sdef(bool, RangeEquals, self s, ssize_t offset, self needle, ssize_t needleOffset);
sdef(bool, BeginsWith, self s, self needle);
sdef(bool, EndsWith, self s, self needle);
def(void, ToLower);
def(void, ToUpper);
overload sdef(bool, Split, self s, char c, String *res);
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
overload sdef(self, Trim, self s, short type);
overload sdef(self, Trim, self s);
overload sdef(self *, SafeTrim, String *s, short type);
overload sdef(String *, SafeTrim, String *s);
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
	.len = sizeof(s) - 1                         \
})

#define StackString(size)        \
	(String) {                   \
		.buf = Arena_AddBuffer(  \
			Arena_GetInstance(), \
			alloca(size), size)  \
	}

#define String_ToNul(s) \
	String_ToNulBuf(s, alloca((s).len + 1))

#undef self
