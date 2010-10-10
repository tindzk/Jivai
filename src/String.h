#import "Bit.h"
#import "Char.h"
#import "NULL.h"
#import "UniStd.h" /* write(), size_t */
#import "VarArg.h"
#import "BitMask.h"
#import "Boolean.h"
#import "Compiler.h"

#ifndef String_SmartAlign
#define String_SmartAlign 1
#endif

typedef struct _String {
	size_t len;
	size_t size;
	char *buf;
	bool mutable;
} String;

enum {
	String_TrimLeft  = Bit(0),
	String_TrimRight = Bit(1)
};

#import "Exception.h"
#import "ExceptionManager.h"

#import "Memory.h"
#import "Module.h"
#import "StringArray.h"

#undef self
#define self String

enum {
	excNotMutable = excOffset,
	excBufferOverflow
};

extern size_t Modules_String;

void String0(ExceptionManager *e);

String HeapString(size_t len);
String BufString(char *buf, size_t len);
void String_Destroy(String *this);
String String_FromNul(char *s);
char* String_ToNulBuf(String s, char *buf);
char* String_ToNulHeap(String s);
String String_Disown(String s);
void String_Resize(String *this, size_t length);
void String_Align(String *this, size_t length);
overload void String_Copy(String *this, String src, ssize_t srcOffset, ssize_t srcLength);
overload void String_Copy(String *this, String src, ssize_t srcOffset);
overload void String_Copy(String *this, String src);
String String_Clone(String s);
char* String_CloneBuf(String s, char *buf);
char String_CharAt(String s, ssize_t offset);
overload String String_Slice(String s, ssize_t offset, ssize_t length);
overload String String_Slice(String s, ssize_t offset);
overload void String_Crop(String *this, ssize_t offset, ssize_t length);
overload void String_Crop(String *this, ssize_t offset);
void String_Delete(String *this, ssize_t offset, ssize_t length);
overload void String_Prepend(String *this, String s);
overload void String_Prepend(String *this, char c);
overload void String_Append(String *this, String s);
overload void String_Append(String *this, char c);
overload void String_Append(String *this, String s, ssize_t offset, ssize_t length);
overload void String_Append(String *this, String s, ssize_t offset);
String String_Join(String *first, ...);
bool String_Equals(String s, String needle);
bool String_RangeEquals(String s, ssize_t offset, String needle, ssize_t needleOffset);
bool String_BeginsWith(String s, String needle);
bool String_EndsWith(String s, String needle);
void String_ToLower(String *this);
void String_ToUpper(String *this);
overload StringArray* String_Split(String s, size_t offset, char c);
overload StringArray* String_Split(String s, char c);
overload ssize_t String_ReverseFind(String s, ssize_t offset, char c);
overload ssize_t String_ReverseFind(String s, char c);
overload ssize_t String_ReverseFind(String s, ssize_t offset, String needle);
overload ssize_t String_ReverseFind(String s, String needle);
overload ssize_t String_Find(String s, ssize_t offset, ssize_t length, char c);
overload ssize_t String_Find(String s, ssize_t offset, ssize_t length, String needle);
overload ssize_t String_Find(String s, String needle);
overload ssize_t String_Find(String s, ssize_t offset, String needle);
overload ssize_t String_Find(String s, char c);
overload ssize_t String_Find(String s, ssize_t offset, char c);
overload bool String_Contains(String s, String needle);
overload bool String_Contains(String s, char needle);
overload void String_Trim(String *this, short type);
overload void String_Trim(String *this);
overload String String_Trim(String s, short type);
overload String String_Trim(String s);
String String_Format(String fmt, ...);
overload ssize_t String_Between(String s, ssize_t offset, String left, String right, bool leftAligned, String *out);
overload ssize_t String_Between(String s, String left, String right, String *out);
overload ssize_t String_Between(String s, ssize_t offset, String left, String right, String *out);
overload String String_Between(String s, ssize_t offset, String left, String right, bool leftAligned);
overload String String_Between(String s, ssize_t offset, String left, String right);
overload String String_Between(String s, String left, String right, bool leftAligned);
overload String String_Between(String s, String left, String right);
String String_Cut(String s, String left, String right);
bool String_Filter(String *this, String s1, String s2);
bool String_Outside(String *this, String left, String right);
String String_Concat(String a, String b);
overload bool String_Replace(String *this, ssize_t offset, String needle, String replacement);
overload bool String_Replace(String *this, String needle, String replacement);
overload String String_Replace(String s, String needle, String replacement);
overload bool String_ReplaceAll(String *this, ssize_t offset, String needle, String replacement);
overload bool String_ReplaceAll(String *this, String needle, String replacement);
overload String String_ReplaceAll(String s, String needle, String replacement);
String String_Consume(String *this, size_t n);
overload void String_Print(String s, bool err);
overload void String_Print(String s);
overload short String_NaturalCompare(String a, String b, bool foldcase, bool skipSpaces, bool skipZeros);
overload short String_NaturalCompare(String a, String b);

#define String_NotFound -1

#define String(s) \
	(String) { sizeof(s) - 1, sizeof(s) - 1, (char *) s, false }

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
