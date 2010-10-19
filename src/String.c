#import "String.h"

size_t Modules_String;

static ExceptionManager *exc;

void String0(ExceptionManager *e) {
	Modules_String = Module_Register(String("String"));

	exc = e;
}

inline String HeapString(size_t len) {
	return (String) {
		.len  = 0,
		.size = len,
		.buf  = (len > 0)
			? Memory_Alloc(len)
			: NULL,
		.mutable = true
	};
}

inline String BufString(char *buf, size_t len) {
	return (String) {
		.len     = len,
		.size    = 0,
		.buf     = buf,
		.mutable = false
	};
}

void String_Destroy(String *this) {
	this->len  = 0;
	this->size = 0;

	if (this->mutable && this->buf != NULL) {
		Memory_Free(this->buf);
	}
}

inline String String_FromNul(char *s) {
	return BufString(s, strlen(s));
}

inline char* String_ToNulBuf(String s, char *buf) {
	if (s.len > 0) {
		Memory_Copy(buf, s.buf, s.len);
	}

	buf[s.len] = '\0';

	return buf;
}

inline char* String_ToNulHeap(String s) {
	return String_ToNulBuf(s, Memory_Alloc(s.len + 1));
}

inline String String_Disown(String s) {
	s.mutable = false;
	return s;
}

void String_Resize(String *this, size_t length) {
	if (!this->mutable) {
		throw(exc, excNotMutable);
	}

	if (length > 0) {
		if (this->buf == NULL) {
			this->buf = Memory_Alloc(length);
		} else {
			this->buf = Memory_Realloc(this->buf, length);
		}
	} else {
		Memory_Free(this->buf);
	}

	this->size = length;

	if (this->len > length) {
		/* The string was shortened. */
		this->len = length;
	}
}

void String_Align(String *this, size_t length) {
	if (length > 0) {
		if (this->size == 0) {
			String_Resize(this, length);
		} else if (length > this->size) {
#if String_SmartAlign
			/* See also:
			 * http://stackoverflow.com/questions/2243366/how-to-implement-a-variable-length-string-y-in-c
			 */
			size_t size = this->size;

			do {
				size <<= 1;
			} while (size < length);

			String_Resize(this, size);
#else
			String_Resize(this, length);
#endif
		}
	}
}

overload void String_Copy(String *this, String src, ssize_t srcOffset, ssize_t srcLength) {
	if (!this->mutable) {
		throw(exc, excNotMutable);
	}

	size_t srcRight;

	if (srcOffset < 0) {
		srcOffset += src.len;
	}

	if (srcLength < 0) {
		srcRight = srcLength + src.len;
	} else {
		srcRight = srcLength + srcOffset;
	}

	if ((size_t) srcOffset > srcRight
	 || (size_t) srcOffset > src.len
	 || srcRight           > src.len) {
		throw(exc, excBufferOverflow);
	}

	String_Align(this, this->len + srcRight - srcOffset);

	if (srcRight - srcOffset > 0) {
		Memory_Copy(this->buf,
			src.buf  + srcOffset,
			srcRight - srcOffset);
	}

	this->len = srcRight - srcOffset;
}

inline overload void String_Copy(String *this, String src, ssize_t srcOffset) {
	String_Copy(this, src, srcOffset, src.len - srcOffset);
}

overload void String_Copy(String *this, String src) {
	if (!this->mutable) {
		throw(exc, excNotMutable);
	}

	if (src.len > 0) {
		if (this->buf == NULL) {
			this->buf = Memory_Alloc(src.len);
		} else if (this->size < src.len) {
			this->size = src.len;
			this->buf  = Memory_Realloc(this->buf, src.len);
		}

		Memory_Copy(this->buf, src.buf, src.len);
	} else if (this->buf != NULL) {
		Memory_Free(this->buf);
	}

	this->len = src.len;
}

String String_Clone(String s) {
	String out = HeapString(s.len);

	if (s.len > 0) {
		Memory_Copy(out.buf, s.buf, s.len);
		out.len = s.len;
	}

	return out;
}

char* String_CloneBuf(String s, char *buf) {
	if (s.len > 0) {
		Memory_Copy(buf, s.buf, s.len);
	}

	return buf;
}

char String_CharAt(String s, ssize_t offset) {
	if (offset < 0) {
		offset += s.len;
	}

	if ((size_t) offset > s.len) {
		throw(exc, excBufferOverflow);
	}

	return s.buf[offset];
}

overload String String_Slice(String s, ssize_t offset, ssize_t length) {
	String out;
	size_t right;

	if (offset < 0) {
		offset += s.len;
	}

	if (length < 0) {
		right = length + s.len;
	} else {
		right = length + offset;
	}

	if ((size_t) offset > right
	 || (size_t) offset > s.len
	 || right           > s.len) {
		throw(exc, excBufferOverflow);
	}

	out.len     = right - offset;
	out.size    = out.len;
	out.buf     = s.buf + offset;
	out.mutable = false;

	return out;
}

inline overload String String_Slice(String s, ssize_t offset) {
	if (offset < 0) {
		offset += s.len;
	}

	return String_Slice(s, offset, s.len - offset);
}

overload void String_Crop(String *this, ssize_t offset, ssize_t length) {
	if (!this->mutable) {
		throw(exc, excNotMutable);
	}

	size_t right;

	if (offset < 0) {
		offset += this->len;
	}

	if (length < 0) {
		right = length + this->len;
	} else {
		right = length + offset;
	}

	if ((size_t) offset > right
	 || (size_t) offset > this->len
	 || right           > this->len) {
		throw(exc, excBufferOverflow);
	}

	if (offset > 0) {
		if (right - offset > 0) {
			/* Memory_Move is preferable because it also works with
			 * overlapping memory areas.
			 */

			Memory_Move(this->buf,
				this->buf + offset,
				right     - offset);
		}

		this->len = right - offset;
	} else {
		this->len = right;
	}
}

inline overload void String_Crop(String *this, ssize_t offset) {
	if (offset < 0) {
		offset += this->len;
	}

	String_Crop(this, offset, this->len - offset);
}

void String_Delete(String *this, ssize_t offset, ssize_t length) {
	if (!this->mutable) {
		throw(exc, excNotMutable);
	}

	size_t from;

	if (offset < 0) {
		offset += this->len;
	}

	from = offset + length;

	if ((size_t) offset > from
	 || (size_t) offset > this->len
	 || from            > this->len) {
		throw(exc, excBufferOverflow);
	}

	Memory_Move(
		this->buf + offset,
		this->buf + from,
		this->len - from);

	this->len = this->len - length;
}

overload void String_Prepend(String *this, String s) {
	String tmp = String_Concat(s, *this);
	String_Copy(this, tmp);
	String_Destroy(&tmp);
}

overload void String_Prepend(String *this, char c) {
	String tmp = HeapString(this->len + 1);
	String_Append(&tmp, c);
	String_Append(&tmp, *this);
	String_Copy(this, tmp);
	String_Destroy(&tmp);
}

overload void String_Append(String *this, String s) {
	if (s.len == 0) {
		return;
	}

	String_Align(this, this->len + s.len);

	Memory_Copy(this->buf + this->len, s.buf, s.len);

	this->len = this->len + s.len;
}

overload void String_Append(String *this, char c) {
	String_Align(this, this->len + 1);
	this->buf[this->len] = c;
	this->len++;
}

String String_Join(String *first, ...) {
	String *s;
	VarArg argptr;
	size_t length = first->len;

	VarArg_Start(argptr, first);

	while (true) {
		s = VarArg_Get(argptr, String *);

		if (s == NULL) {
			break;
		}

		length += s->len;
	}

	VarArg_End(argptr);

	String res = HeapString(length);

	VarArg_Start(argptr, first);

	s = first;

	while (true) {
		if (s->len > 0) {
			Memory_Copy(res.buf + res.len, s->buf, s->len);
			res.len += s->len;
		}

		s = VarArg_Get(argptr, String *);

		if (s == NULL) {
			break;
		}
	}

	VarArg_End(argptr);

	return res;
}

inline bool String_Equals(String s, String needle) {
	return s.len == needle.len && Memory_Equals(s.buf, needle.buf, s.len);
}

bool String_RangeEquals(String s, ssize_t offset, String needle, ssize_t needleOffset) {
	if (needle.len == 0) {
		return true;
	}

	if (needle.len > s.len) {
		return false;
	}

	if (offset < 0) {
		offset += s.len;
	}

	if (needleOffset < 0) {
		needleOffset += needle.len;
	}

	if ((size_t) offset > s.len) {
		return false;
	}

	if ((size_t) needleOffset > needle.len) {
		return false;
	}

	return Memory_Equals(
		s.buf + offset,
		needle.buf + needleOffset,
		needle.len - needleOffset);
}

inline bool String_BeginsWith(String s, String needle) {
	return String_RangeEquals(s, 0, needle, 0);
}

inline bool String_EndsWith(String s, String needle) {
	return String_RangeEquals(s, s.len - needle.len, needle, 0);
}

void String_ToLower(String *this) {
	if (!this->mutable) {
		throw(exc, excNotMutable);
	}

	for (size_t i = 0; i < this->len; i++) {
		this->buf[i] = (char) Char_ToLower(this->buf[i]);
	}
}

void String_ToUpper(String *this) {
	if (!this->mutable) {
		throw(exc, excNotMutable);
	}

	for (size_t i = 0; i < this->len; i++) {
		this->buf[i] = (char) Char_ToUpper(this->buf[i]);
	}
}

overload StringArray* String_Split(String s, size_t offset, char c) {
	size_t chunks = 1;
	size_t left, right;

	for (size_t i = offset; i < s.len; i++) {
		if (s.buf[i] == c) {
			chunks++;
		}
	}

	StringArray *res = StringArray_New(chunks);

	for (left = right = offset; right < s.len; right++) {
		if (s.buf[right] == c) {
			res->buf[res->len] = String_Slice(s, left, right - left);
			res->len++;
			left = right + 1;
		}
	}

	res->buf[res->len] = String_Slice(s, left, right - left);
	res->len++;

	return res;
}

inline overload StringArray* String_Split(String s, char c) {
	return String_Split(s, 0, c);
}

overload ssize_t String_Find(String s, ssize_t offset, ssize_t length, char c) {
	size_t right;

	if (offset < 0) {
		offset += s.len;
	}

	if (length < 0) {
		right = length + s.len;
	} else {
		right = length + offset;
	}

	for (size_t i = offset; i < right; i++) {
		if (s.buf[i] == c) {
			return i;
		}
	}

	return String_NotFound;
}

overload ssize_t String_ReverseFind(String s, ssize_t offset, char c) {
	if (s.len == 0) {
		return String_NotFound;
	}

	if (offset < 0) {
		offset += s.len - 1;
	}

	if ((size_t) offset > s.len) {
		throw(exc, excBufferOverflow);
	}

	for (ssize_t i = offset; i >= 0; i--) {
		if (s.buf[i] == c) {
			return i;
		}
	}

	return String_NotFound;
}

inline overload ssize_t String_ReverseFind(String s, char c) {
	return String_ReverseFind(s, s.len - 1, c);
}

overload ssize_t String_ReverseFind(String s, ssize_t offset, String needle) {
	if (s.len == 0) {
		return String_NotFound;
	}

	if (offset < 0) {
		offset += s.len - 1;
	}

	if ((size_t) offset > s.len) {
		throw(exc, excBufferOverflow);
	}

	size_t cnt = 0;

	for (ssize_t i = offset; i >= 0; i--) {
		if (s.buf[i] == needle.buf[needle.len - cnt - 1]) {
			cnt++;

			if (cnt == needle.len) {
				return i;
			}
		} else {
			if (cnt > 0) {
				cnt = 0;
				i++;
			}
		}
	}

	return String_NotFound;
}

inline overload ssize_t String_ReverseFind(String s, String needle) {
	return String_ReverseFind(s, s.len - 1, needle);
}

overload ssize_t String_Find(String s, ssize_t offset, ssize_t length, String needle) {
	size_t right;

	if (offset < 0) {
		offset += s.len;
	}

	if (length < 0) {
		right = length + s.len;
	} else {
		right = length + offset;
	}

	if ((size_t) offset > s.len
	 || (size_t) right  > s.len) {
		throw(exc, excBufferOverflow);
	}

	size_t cnt = 0;

	for (size_t i = offset; i < right; i++) {
		if (s.buf[i] == needle.buf[cnt]) {
			cnt++;

			if (cnt == needle.len) {
				return i - needle.len + 1;
			}
		} else {
			if (cnt > 0) {
				cnt = 0;
				i--;
			}
		}
	}

	return String_NotFound;
}

inline overload ssize_t String_Find(String s, String needle) {
	return String_Find(s, 0, s.len, needle);
}

inline overload ssize_t String_Find(String s, ssize_t offset, String needle) {
	if (offset < 0) {
		offset += s.len;
	}

	return String_Find(s, offset, s.len - offset, needle);
}

inline overload ssize_t String_Find(String s, char c) {
	return String_Find(s, 0, s.len, c);
}

inline overload ssize_t String_Find(String s, ssize_t offset, char c) {
	if (offset < 0) {
		offset += s.len;
	}

	return String_Find(s, offset, s.len - offset, c);
}

overload bool String_Contains(String s, String needle) {
	return String_Find(s, 0, s.len, needle) != String_NotFound;
}

overload bool String_Contains(String s, char needle) {
	return String_Find(s, 0, s.len, needle) != String_NotFound;
}

overload void String_Trim(String *this, short type) {
	size_t i, lpos = 0;

	if (BitMask_Has(type, String_TrimLeft)) {
		for (i = 0; i < this->len; i++) {
			if (Char_IsSpace(this->buf[i])) {
				lpos = i + 1;
			} else {
				break;
			}
		}
	}

	if (lpos == this->len) {
		this->len = 0;
	} else {
		size_t rpos = this->len;

		if (BitMask_Has(type, String_TrimRight)) {
			for (i = rpos; i > 0; i--) {
				if (Char_IsSpace(this->buf[i - 1])) {
					rpos = i - 1;
				} else {
					break;
				}
			}
		}

		String_Crop(this, lpos, rpos - lpos);
	}
}

inline overload void String_Trim(String *this) {
	String_Trim(this,
		String_TrimLeft |
		String_TrimRight);
}

overload String String_Trim(String s, short type) {
	size_t i, lpos = 0;

	if (BitMask_Has(type, String_TrimLeft)) {
		for (i = 0; i < s.len; i++) {
			if (Char_IsSpace(s.buf[i])) {
				lpos = i + 1;
			} else {
				break;
			}
		}
	}

	if (lpos == s.len) {
		s.len = 0;
	} else {
		size_t rpos = s.len;

		if (BitMask_Has(type, String_TrimRight)) {
			for (i = rpos; i > 0; i--) {
				if (Char_IsSpace(s.buf[i - 1])) {
					rpos = i - 1;
				} else {
					break;
				}
			}
		}

		return String_Slice(s, lpos, rpos - lpos);
	}

	return s;
}

inline overload String String_Trim(String s) {
	return String_Trim(s,
		String_TrimLeft |
		String_TrimRight);
}

String String_Format(String fmt, ...) {
	size_t length = 0;
	VarArg argptr;
	String param;

	VarArg_Start(argptr, fmt);

	for (size_t i = 0; i < fmt.len; i++) {
		if (fmt.buf[i] == '%') {
			if (i == 0 || fmt.buf[i - 1] != '!') {
				length += VarArg_Get(argptr, String).len;
			}
		} else {
			length++;
		}
	}

	VarArg_End(argptr);

	String res = HeapString(length);

	VarArg_Start(argptr, fmt);

	for (size_t i = 0; i < fmt.len; i++) {
		if (i + 1 != fmt.len && fmt.buf[i] == '!' && fmt.buf[i + 1] == '%') {
			res.buf[res.len] = '%';
			res.len++;
		} else if (fmt.buf[i] == '%') {
			param = VarArg_Get(argptr, String);

			if (param.len > 0) {
				Memory_Copy(res.buf + res.len, param.buf, param.len);
				res.len += param.len;
			}
		} else {
			res.buf[res.len] = fmt.buf[i];
			res.len++;
		}
	}

	VarArg_End(argptr);

	return res;
}

overload ssize_t String_Between(String s, ssize_t offset, String left, String right, bool leftAligned, String *out) {
	ssize_t posLeft, posRight;

	if (offset < 0) {
		offset += s.len;
	}

	if (leftAligned) {
		if ((posLeft = String_Find(s, offset, left)) == String_NotFound) {
			return String_NotFound;
		}

		posLeft += left.len;

		if ((posRight = String_Find(s, posLeft, right)) == String_NotFound) {
			return String_NotFound;
		}
	} else {
		if ((posRight = String_Find(s, offset, right)) == String_NotFound) {
			return String_NotFound;
		}

		if (posRight > 0) {
			if ((posLeft = String_ReverseFind(s, posRight - 1, left)) == String_NotFound) {
				return String_NotFound;
			}

			posLeft += left.len;
		} else {
			return String_NotFound;
		}
	}

	*out = BufString(s.buf + posLeft, posRight - posLeft);

	return posRight + right.len;
}

inline overload ssize_t String_Between(String s, String left, String right, String *out) {
	return String_Between(s, 0, left, right, true, out);
}

inline overload ssize_t String_Between(String s, ssize_t offset, String left, String right, String *out) {
	return String_Between(s, offset, left, right, true, out);
}

inline overload String String_Between(String s, ssize_t offset, String left, String right, bool leftAligned) {
	String out = StackString(0);
	String_Between(s, offset, left, right, leftAligned, &out);
	return out;
}

inline overload String String_Between(String s, ssize_t offset, String left, String right) {
	String out = StackString(0);
	String_Between(s, offset, left, right, &out);
	return out;
}

inline overload String String_Between(String s, String left, String right, bool leftAligned) {
	String out = StackString(0);
	String_Between(s, 0, left, right, leftAligned, &out);
	return out;
}

inline overload String String_Between(String s, String left, String right) {
	String out = StackString(0);
	String_Between(s, 0, left, right, true, &out);
	return out;
}

String String_Cut(String s, String left, String right) {
	ssize_t posLeft = String_Find(s, left);

	if (posLeft == String_NotFound) {
		return String("");
	}

	ssize_t posRight = String_Find(s, posLeft + left.len, right);

	if (posRight == String_NotFound) {
		return String("");
	}

	return String_Slice(s, posLeft, posRight - posLeft);
}

bool String_Filter(String *this, String s1, String s2) {
	ssize_t left, right;

	if ((left = String_Find(*this, s1)) == String_NotFound) {
		return false;
	}

	String out = HeapString(0);

	if (left > 0) {
		out = String_Clone(String_Slice(*this, 0, left - 1));
	}

	left += s1.len;

	if ((right = String_Find(*this, left, s2)) == String_NotFound) {
		String_Destroy(&out);
		return false;
	}

	String_Append(&out, String_Slice(*this, left, right - left));
	String_Append(&out, String_Slice(*this, right + s2.len));

	String_Copy(this, out);

	String_Destroy(&out);

	return true;
}

bool String_Outside(String *this, String left, String right) {
	ssize_t posLeft, posRight;

	if ((posLeft = String_Find(*this, left)) == String_NotFound) {
		return false;
	}

	if ((posRight = String_Find(*this, posLeft + left.len, right)) == String_NotFound) {
		return false;
	}

	String out = HeapString(posLeft + this->len - posRight - right.len);

	String_Append(&out, String_Slice(*this, 0, posLeft));
	String_Append(&out, String_Slice(*this, posRight + right.len));

	String_Copy(this, out);

	String_Destroy(&out);

	return true;
}

overload String String_Concat(String a, String b) {
	String res = HeapString(a.len + b.len);

	if (a.len > 0) {
		Memory_Copy(res.buf, a.buf, a.len);
		res.len = a.len;
	}

	if (b.len > 0) {
		Memory_Copy(res.buf + res.len, b.buf, b.len);
		res.len += b.len;
	}

	return res;
}

overload String String_Concat(String s, char c) {
	String res = HeapString(s.len + 1);

	if (s.len > 0) {
		Memory_Copy(res.buf, s.buf, s.len);
	}

	res.buf[s.len] = c;

	res.len = s.len + 1;

	return res;
}

overload bool String_Replace(String *this, ssize_t offset, String needle, String replacement) {
	if (offset < 0) {
		offset += this->len;
	}

	ssize_t pos = String_Find(*this, offset, needle);

	if (pos == String_NotFound) {
		return false;
	}

	ssize_t len = this->len - needle.len + replacement.len;

	if (len < 0) {
		len = replacement.len;
	}

	String out = HeapString(len);

	String_Append(&out, String_Slice(*this, 0, pos));
	String_Append(&out, replacement);
	String_Append(&out, String_Slice(*this, pos + needle.len));

	String_Copy(this, out);

	String_Destroy(&out);

	return true;
}

inline overload bool String_Replace(String *this, String needle, String replacement) {
	return String_Replace(this, 0, needle, replacement);
}

inline overload String String_Replace(String s, String needle, String replacement) {
	String tmp = s;
	String_Replace(&tmp, 0, needle, replacement);
	return tmp;
}

overload bool String_ReplaceAll(String *this, ssize_t offset, String needle, String replacement) {
	if (offset < 0) {
		offset += this->len;
	}

	ssize_t len = this->len - needle.len + replacement.len;

	if (len < 0) {
		len = replacement.len;
	}

	/* Approximation for one occurence. */
	String out = HeapString(len);

	size_t cnt     = 0;
	size_t lastPos = 0;

	for (size_t i = offset; i < this->len; i++) {
		if (this->buf[i] == needle.buf[cnt]) {
			cnt++;

			if (cnt == needle.len) {
				size_t cur = i - needle.len + 1;

				String_Append(&out, String_Slice(*this, lastPos, cur - lastPos));
				String_Append(&out, replacement);

				lastPos = i + 1;
				cnt = 0;
			}
		} else {
			if (cnt > 0) {
				cnt = 0;
				i--;
			}
		}
	}

	String_Append(&out, String_Slice(*this, lastPos));

	String_Copy(this, out);

	String_Destroy(&out);

	return lastPos != 0;
}

inline overload bool String_ReplaceAll(String *this, String needle, String replacement) {
	return String_ReplaceAll(this, 0, needle, replacement);
}

inline overload String String_ReplaceAll(String s, String needle, String replacement) {
	String tmp = s;
	String_ReplaceAll(&tmp, 0, needle, replacement);
	return tmp;
}

String String_Consume(String *this, size_t n) {
	String res = String_Clone(String_Slice(*this, 0, n));
	String_Crop(this, n);
	return res;
}

overload void String_Print(String s, bool err) {
	if (s.buf != NULL) {
		Kernel_write(err ? FileNo_StdErr : FileNo_StdOut, s.buf, s.len);
	}
}

inline overload void String_Print(String s) {
	String_Print(s, false);
}

/*
 * String_CompareRight(), String_CompareLeft() and String_NaturalCompare()
 * are based upon Martin Pool's `strnatcmp' library:
 *
 * strnatcmp.c -- Perform 'natural order' comparisons of strings in C.
 * Copyright (C) 2000, 2004 by Martin Pool <mbp sourcefrog net>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

short String_CompareRight(String a, String b) {
	short bias = 0;

	/* The longest run of digits wins.  That aside, the greatest
	 * value wins, but we can't know that it will until we've scanned
	 * both numbers to know that they have the same magnitude, so we
	 * remember it in `bias'.
	 */

	for (size_t i = 0; ; i++) {
		if (i >= a.len && i >= b.len) {
			return bias;
		} else if (!Char_IsDigit(a.buf[i])
				&& !Char_IsDigit(b.buf[i]))
		{
			return bias;
		} else if (!Char_IsDigit(a.buf[i])) {
			return -1;
		} else if (!Char_IsDigit(b.buf[i])) {
			return 1;
		} else if (a.buf[i] < b.buf[i]) {
			if (bias == 0) {
				bias = -1;
			}
		} else if (a.buf[i] > b.buf[i]) {
			if (bias == 0) {
				bias = 1;
			}
		}
	}

	return bias;
}

short String_CompareLeft(String a, String b) {
	/* Compare two left-aligned numbers: the first to have a
	 * different value wins.
	 */

	for (size_t i = 0; i < a.len && i < b.len; i++) {
		if (!Char_IsDigit(a.buf[i])
		 && !Char_IsDigit(b.buf[i])) {
			return 0;
		} else if (!Char_IsDigit(a.buf[i])) {
			return -1;
		} else if (!Char_IsDigit(b.buf[i])) {
			return 1;
		} else if (a.buf[i] < b.buf[i]) {
			return -1;
		} else if (a.buf[i] > b.buf[i]) {
			return 1;
		}
	}

	return 0;
}

overload short String_NaturalCompare(String a, String b, bool foldcase, bool skipSpaces, bool skipZeros) {
	size_t ai = 0;
	size_t bi = 0;

	while (ai < a.len && bi < b.len) {
		/* Skip over leading spaces (and zeros, if desired). */
		while ((skipSpaces && Char_IsSpace(a.buf[ai]))
			|| (skipZeros && a.buf[ai] == '0'))
		{
			ai++;
		}

		while ((skipSpaces && Char_IsSpace(b.buf[bi]))
			|| (skipZeros && b.buf[bi] == '0'))
		{
			bi++;
		}

		/* Process run of digits. */
		if (Char_IsDigit(a.buf[ai]) && Char_IsDigit(b.buf[bi])) {
			short result;

			String __a = String_Slice(a, ai);
			String __b = String_Slice(b, bi);

			if (!skipZeros) {
				/* Is fractional? */
				if (a.buf[ai] == '0' || b.buf[bi] == '0') {
					result = String_CompareLeft(__a, __b);

					if (result != 0) {
						return result;
					}
				}
			}

			result = String_CompareRight(__a, __b);

			if (result != 0) {
				return result;
			}
		}

		char ca = a.buf[ai];
		char cb = b.buf[bi];

		if (foldcase) {
			ca = (char) Char_ToLower(ca);
			cb = (char) Char_ToLower(cb);
		}

		if (ca < cb) {
			return -1;
		} else if (ca > cb) {
			return 1;
		}

		ai++;
		bi++;
	}

	if (ai < bi) {
		return -1;
	} else if (ai > bi) {
		return 1;
	} else {
		return 0;
	}
}

inline overload short String_NaturalCompare(String a, String b) {
	return String_NaturalCompare(a, b, true, true, true);
}

void StringArray_Destroy(StringArray *this) {
	for (size_t i = 0; i < this->len; i++) {
		String_Destroy(&this->buf[i]);
	}

	this->len = 0;
}

void StringArray_ToHeap(StringArray *this) {
	for (size_t i = 0; i < this->len; i++) {
		if (!this->buf[i].mutable) {
			this->buf[i] = String_Clone(this->buf[i]);
		}
	}
}
