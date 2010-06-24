#ifndef STRING_H
#define STRING_H

#include "UniStd.h" /* write() */
#include "NULL.h"
#include "Char.h"
#include "VarArg.h"
#include "Compiler.h"

typedef struct _String {
	size_t len;
	size_t size;
	char *buf;
	bool heap;
} String;

#include "Memory.h"
#include "Exception.h"
#include "StringArray.h"
#include "ExceptionManager.h"

Exception_Export(String_BufferOverflowException);
Exception_Export(String_NotHeapAllocatedException);

void String0(ExceptionManager *e);

void String_Destroy(String *this);
void String_ToHeap(String *this);
char* String_ToNulBuf(String *this, char *buf);
void String_Resize(String *this, size_t length);
void String_Align(String *this, size_t length);
void OVERLOAD String_Copy(String *this, String src, ssize_t srcOffset, ssize_t srcLength);
void OVERLOAD String_Copy(String *this, String src, ssize_t srcOffset);
void OVERLOAD String_Copy(String *this, String src);
String String_Clone(String s);
char* String_CloneBuf(String s, char *buf);
char String_CharAt(String *this, ssize_t offset);
String OVERLOAD String_Slice(String *this, ssize_t offset, ssize_t length);
String OVERLOAD String_Slice(String *this, ssize_t offset);
String OVERLOAD String_FastSlice(String *this, ssize_t offset, ssize_t length);
String OVERLOAD String_FastSlice(String *this, ssize_t offset);
void OVERLOAD String_Crop(String *this, ssize_t offset, ssize_t length);
void OVERLOAD String_Crop(String *this, ssize_t offset);
void String_Delete(String *this, ssize_t offset, ssize_t length);
void OVERLOAD String_Append(String *this, String s);
void OVERLOAD String_Append(String *this, char c);
void OVERLOAD String_Append(String *this, String s, ssize_t offset, ssize_t length);
void OVERLOAD String_Append(String *this, String s, ssize_t offset);
String String_Join(String *first, ...);
bool String_RangeEquals(String *this, ssize_t offset, String needle, ssize_t needleOffset);
void String_ToLower(String *this);
void String_ToUpper(String *this);
StringArray OVERLOAD String_SplitChar(String *this, size_t offset, char c);
StringArray OVERLOAD String_SplitChar(String *this, char c);
ssize_t String_ReverseFindChar(String *this, char c);
static inline OVERLOAD ssize_t String_FindRange(String *this, ssize_t offset, ssize_t length, char c);
static inline OVERLOAD ssize_t String_FindRange(String *this, ssize_t offset, ssize_t length, String needle);
ssize_t OVERLOAD String_Find(String *this, String needle);
ssize_t OVERLOAD String_Find(String *this, ssize_t offset, String needle);
ssize_t OVERLOAD String_Find(String *this, ssize_t offset, ssize_t length, String needle);
ssize_t OVERLOAD String_Find(String *this, char c);
ssize_t OVERLOAD String_Find(String *this, ssize_t offset, char c);
ssize_t OVERLOAD String_Find(String *this, ssize_t offset, ssize_t length, char c);
bool String_Contains(String *this, String needle);
void String_Trim(String *this);
void String_TrimLeft(String *this);
String String_Format(String fmt, ...);
ssize_t OVERLOAD String_Between(String *this, ssize_t offset, String left, String right, String *out);
ssize_t OVERLOAD String_Between(String *this, String left, String right, String *out);
String OVERLOAD String_Between(String *this, ssize_t offset, String left, String right);
String OVERLOAD String_Between(String *this, String left, String right);
bool String_Filter(String *this, String s1, String s2);
bool String_Outside(String *this, String left, String right);
String String_Concat(String a, String b);
bool String_Replace(String *this, String needle, String replacement);
String String_Consume(String *this, int n);
void String_Print(String s);

#define String_NotFound -1

#define String(s) \
	(String) { sizeof(s) - 1, sizeof(s) - 1, s, false }

#define PtrString(s) \
	&String(s)

#define StaticString(s) \
	{ sizeof(s) - 1, sizeof(s) - 1, s, false }

#define HeapString(len) \
	(String) { 0, len, (len > 0) ? Memory_Alloc(len) : NULL, true }

#define BufString(buf, len) \
	(String) { len, 0, buf, false }

#define StackString(len) \
	(String) { 0, len, (len > 0) ? alloca(len) : NULL, false }

#define String_StackClone(s) \
	(String) { (s).len, (s).len, String_CloneBuf(s, alloca((s).len)), false }

#define String_ToNul(this) \
	String_ToNulBuf(this, alloca(((this)->len + 1)))

#define String_Equals(this, needle) \
	((this)->len == (needle).len && Memory_Equals((this)->buf, (needle).buf, (this)->len))

#define String_BeginsWith(this, needle) \
	String_RangeEquals(this, 0, needle, 0)

#define String_EndsWith(this, needle) \
	String_RangeEquals(this, (this)->len - (needle).len, needle, 0)

#endif
