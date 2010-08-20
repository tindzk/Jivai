#import "UniStd.h" /* write() */
#import "NULL.h"
#import "Char.h"
#import "VarArg.h"
#import "Compiler.h"

typedef struct _String {
	size_t len;
	size_t size;
	char *buf;
	bool mutable;
} String;

#import "Memory.h"
#import "Exception.h"
#import "StringArray.h"
#import "ExceptionManager.h"

#undef self
#define self String

Exception_Export(NotMutableException);
Exception_Export(BufferOverflowException);

void String0(ExceptionManager *e);

void String_Destroy(String *this);
char* String_ToNulBuf(String s, char *buf);
void String_Resize(String *this, size_t length);
void String_Align(String *this, size_t length);
void OVERLOAD String_Copy(String *this, String src, ssize_t srcOffset, ssize_t srcLength);
void OVERLOAD String_Copy(String *this, String src, ssize_t srcOffset);
void OVERLOAD String_Copy(String *this, String src);
String String_Clone(String s);
char* String_CloneBuf(String s, char *buf);
char String_CharAt(String s, ssize_t offset);
String OVERLOAD String_Slice(String s, ssize_t offset, ssize_t length);
String OVERLOAD String_Slice(String s, ssize_t offset);
void OVERLOAD String_Crop(String *this, ssize_t offset, ssize_t length);
void OVERLOAD String_Crop(String *this, ssize_t offset);
void String_Delete(String *this, ssize_t offset, ssize_t length);
void OVERLOAD String_Append(String *this, String s);
void OVERLOAD String_Append(String *this, char c);
void OVERLOAD String_Append(String *this, String s, ssize_t offset, ssize_t length);
void OVERLOAD String_Append(String *this, String s, ssize_t offset);
String String_Join(String *first, ...);
bool String_Equals(String s, String needle);
bool String_RangeEquals(String s, ssize_t offset, String needle, ssize_t needleOffset);
bool String_BeginsWith(String s, String needle);
bool String_EndsWith(String s, String needle);
void String_ToLower(String *this);
void String_ToUpper(String *this);
StringArray* OVERLOAD String_Split(String s, size_t offset, char c);
StringArray* OVERLOAD String_Split(String s, char c);
ssize_t OVERLOAD String_ReverseFind(String s, ssize_t offset, char c);
ssize_t OVERLOAD String_ReverseFind(String s, char c);
ssize_t OVERLOAD String_ReverseFind(String s, ssize_t offset, String needle);
ssize_t OVERLOAD String_ReverseFind(String s, String needle);
ssize_t OVERLOAD String_Find(String s, ssize_t offset, ssize_t length, char c);
ssize_t OVERLOAD String_Find(String s, ssize_t offset, ssize_t length, String needle);
ssize_t OVERLOAD String_Find(String s, String needle);
ssize_t OVERLOAD String_Find(String s, ssize_t offset, String needle);
ssize_t OVERLOAD String_Find(String s, char c);
ssize_t OVERLOAD String_Find(String s, ssize_t offset, char c);
bool OVERLOAD String_Contains(String s, String needle);
bool OVERLOAD String_Contains(String s, char needle);
void OVERLOAD String_Trim(String *this);
String OVERLOAD String_Trim(String s);
void String_TrimLeft(String *this);
String String_Format(String fmt, ...);
ssize_t OVERLOAD String_Between(String s, ssize_t offset, String left, String right, bool leftAligned, String *out);
ssize_t OVERLOAD String_Between(String s, String left, String right, String *out);
ssize_t OVERLOAD String_Between(String s, ssize_t offset, String left, String right, String *out);
String OVERLOAD String_Between(String s, ssize_t offset, String left, String right, bool leftAligned);
String OVERLOAD String_Between(String s, ssize_t offset, String left, String right);
String OVERLOAD String_Between(String s, String left, String right, bool leftAligned);
String OVERLOAD String_Between(String s, String left, String right);
bool String_Filter(String *this, String s1, String s2);
bool String_Outside(String *this, String left, String right);
String String_Concat(String a, String b);
bool OVERLOAD String_Replace(String *this, ssize_t offset, String needle, String replacement);
bool OVERLOAD String_Replace(String *this, String needle, String replacement);
bool OVERLOAD String_ReplaceAll(String *this, ssize_t offset, String needle, String replacement);
bool OVERLOAD String_ReplaceAll(String *this, String needle, String replacement);
String String_Consume(String *this, size_t n);
void String_Print(String s);
short OVERLOAD String_NaturalCompare(String a, String b, bool foldcase, bool skipSpaces, bool skipZeros);
short OVERLOAD String_NaturalCompare(String a, String b);

#define String_NotFound -1

#define String(s) \
	(String) { sizeof(s) - 1, sizeof(s) - 1, s, false }

#define HeapString(len) \
	(String) { 0, len, (len > 0) ? Memory_Alloc(len) : NULL, true }

#define BufString(buf, len) \
	(String) { len, 0, buf, false }

#define StackString(len) \
	(String) { 0, len, (len > 0) ? alloca(len) : NULL, true }

#define String_StackClone(s) \
	(String) { (s).len, (s).len, String_CloneBuf(s, alloca((s).len)), true }

#define String_FromNul(s) \
	BufString(s, strlen(s))

#define String_ToNul(s) \
	String_ToNulBuf(s, alloca((s).len + 1))

#define String_ToNulHeap(s) \
	String_ToNulBuf(s, Memory_Alloc((s).len + 1))

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
