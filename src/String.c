#include "String.h"

Exception_Define(String_BufferOverflowException);
Exception_Define(String_NotHeapAllocatedException);

static ExceptionManager *exc;

void String0(ExceptionManager *e) {
	exc = e;
}

void String_Destroy(String *this) {
	this->len  = 0;
	this->size = 0;

	if (this->heap && this->buf != NULL) {
		Memory_Free(this->buf);
	}
}

void String_ToHeap(String *this) {
	char *buf;

	if (this->heap) {
		return;
	}

	buf = Memory_Alloc(this->size);
	Memory_Copy(buf, this->buf, this->len);

	this->buf  = buf;
	this->heap = true;
}

char* String_ToNulBuf(String *this, char *buf) {
	size_t i;

	for (i = 0; i < this->len; i++) {
		buf[i] = this->buf[i];
	}

	buf[i] = '\0';

	return buf;
}

void String_Resize(String *this, size_t length) {
	if (!this->heap) {
		throw(exc, &String_NotHeapAllocatedException);
	}

	if (this->buf == NULL) {
		this->buf = Memory_Alloc(length);
	} else {
		this->buf = Memory_Realloc(this->buf, length);
	}

	this->size = length;

	if (this->len > length) {
		/* The string was shortened. */
		this->len = length;
	}
}

void String_Align(String *this, size_t length) {
	if (length > 0) {
		if (this->size == 0 || length > this->size) {
			String_Resize(this, length);
		}
	}
}

void OVERLOAD String_Copy(String *this, String src, ssize_t srcOffset, ssize_t srcLength) {
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
		throw(exc, &String_BufferOverflowException);
	}

	String_Align(this, this->len + srcRight - srcOffset);

	if (srcRight - srcOffset > 0) {
		Memory_Copy(this->buf,
			src.buf  + srcOffset,
			srcRight - srcOffset);
	}

	this->len = srcRight - srcOffset;
}

void OVERLOAD String_Copy(String *this, String src, ssize_t srcOffset) {
	String_Copy(this, src, srcOffset, src.len - srcOffset);
}

void OVERLOAD String_Copy(String *this, String src) {
	if (src.len > 0) {
		if (!this->heap) {
			throw(exc, &String_NotHeapAllocatedException);
		}

		if (this->buf == NULL) {
			this->buf = Memory_Alloc(src.len);
		} else if (this->size < src.len) {
			this->size = src.len;
			this->buf  = Memory_Realloc(this->buf, src.len);
		}

		Memory_Copy(this->buf, src.buf, src.len);
	} else if (this->buf != NULL) {
		if (this->heap) {
			Memory_Free(this->buf);
		} else {
			this->buf = NULL;
		}
	}

	this->len  = src.len;
	this->heap = true;
}

String String_Clone(String s) {
	if (s.len == 0) {
		return HeapString(0);
	}

	String out;

	out.len = s.len;
	out.size = s.len;
	out.buf = Memory_Alloc(out.size);
	Memory_Copy(out.buf, s.buf, s.len);
	out.heap = true;

	return out;
}

char* String_CloneBuf(String s, char *buf) {
	if (s.len > 0) {
		Memory_Copy(buf, s.buf, s.len);
	}

	return buf;
}

char String_CharAt(String *this, ssize_t offset) {
	if (offset < 0) {
		offset += this->len;
	}

	if ((size_t) offset > this->len) {
		throw(exc, &String_BufferOverflowException);
	}

	return this->buf[offset];
}

String OVERLOAD String_Slice(String *this, ssize_t offset, ssize_t length) {
	String out;
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
		throw(exc, &String_BufferOverflowException);
	}

	out.len  = right - offset;
	out.size = out.len;

	if (out.len == 0) {
		out.buf = NULL;
	} else {
		out.buf = Memory_Alloc(out.size);
		Memory_Copy(out.buf, this->buf + offset, out.len);
	}

	out.heap = true;

	return out;
}

String OVERLOAD String_Slice(String *this, ssize_t offset) {
	return String_Slice(this, offset, this->len - offset);
}

void OVERLOAD String_Crop(String *this, ssize_t offset, ssize_t length) {
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
		throw(exc, &String_BufferOverflowException);
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

void OVERLOAD String_Crop(String *this, ssize_t offset) {
	String_Crop(this, offset, this->len - offset);
}

void String_Delete(String *this, ssize_t offset, ssize_t length) {
	size_t from;

	if (offset < 0) {
		offset += this->len;
	}

	from = offset + length;

	if ((size_t) offset > from
	 || (size_t) offset > this->len
	 || from            > this->len) {
		throw(exc, &String_BufferOverflowException);
	}

	if (!this->heap) {
		throw(exc, &String_NotHeapAllocatedException);
	}

	Memory_Move(
		this->buf + offset,
		this->buf + from,
		this->len - from);

	this->len = this->len - length;
}

void OVERLOAD String_Append(String *this, String s) {
	if (s.len == 0) {
		return;
	}

	String_Align(this, this->len + s.len);

	Memory_Copy(this->buf + this->len, s.buf, s.len);

	this->len = this->len + s.len;
}

void OVERLOAD String_Append(String *this, String s, ssize_t offset, ssize_t length) {
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
		throw(exc, &String_BufferOverflowException);
	}

	length = right - offset;

	if (length > 0) {
		String_Align(this, this->len + length);
		Memory_Copy(this->buf + this->len, s.buf + offset, length);
		this->len += length;
	}
}

void OVERLOAD String_Append(String *this, String s, ssize_t offset) {
	String_Append(this, s, offset, s.len - offset);
}

void OVERLOAD String_Append(String *this, char c) {
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

bool String_RangeEquals(String *this, ssize_t offset, String needle, ssize_t needleOffset) {
	if (this->len == 0 || needle.len == 0) {
		return false;
	}

	if (needle.len > this->len) {
		return false;
	}

	if (offset < 0) {
		offset += this->len;
	}

	if (needleOffset < 0) {
		needleOffset += needle.len;
	}

	if ((size_t) offset > this->len) {
		return false;
	}

	if ((size_t) needleOffset > needle.len) {
		return false;
	}

	return Memory_Equals(
		this->buf  + offset,
		needle.buf + needleOffset,
		needle.len - needleOffset);
}

void String_ToLower(String *this) {
	for (size_t i = 0; i < this->len; i++) {
		this->buf[i] = Char_ToLower(this->buf[i]);
	}
}

void String_ToUpper(String *this) {
	for (size_t i = 0; i < this->len; i++) {
		this->buf[i] = Char_ToUpper(this->buf[i]);
	}
}

StringArray OVERLOAD String_SplitChar(String *this, size_t offset, char c) {
	size_t chunks = 1;
	size_t left, right;

	for (size_t i = offset; i < this->len; i++) {
		if (this->buf[i] == c) {
			chunks++;
		}
	}

	StringArray res;
	Array_Init(&res, chunks);

	for (left = right = offset; right < this->len; right++) {
		if (this->buf[right] == c) {
			res.buf[res.len] = String_Slice(this, left, right - left);
			res.len++;

			left = right + 1;
		}
	}

	res.buf[res.len] = String_Slice(this, left, right - left);
	res.len++;

	return res;
}

StringArray OVERLOAD String_SplitChar(String *this, char c) {
	return String_SplitChar(this, 0, c);
}

static inline OVERLOAD ssize_t String_FindRange(String *this, ssize_t offset, ssize_t length, char c) {
	size_t right;

	if (offset < 0) {
		offset += this->len;
	}

	if (length < 0) {
		right = length + this->len;
	} else {
		right = length + offset;
	}

	for (size_t i = offset; i < right; i++) {
		if (this->buf[i] == c) {
			return i;
		}
	}

	return String_NotFound;
}

ssize_t String_ReverseFindChar(String *this, char c) {
	for (ssize_t i = this->len - 1; i >= 0; i--) {
		if (this->buf[i] == c) {
			return i;
		}
	}

	return String_NotFound;
}

static inline OVERLOAD ssize_t String_FindRange(String *this, ssize_t offset, ssize_t length, String needle) {
	size_t right;

	if (offset < 0) {
		offset += this->len;
	}

	if (length < 0) {
		right = length + this->len;
	} else {
		right = length + offset;
	}

	size_t cnt = 0;

	for (size_t i = offset; i < right; i++) {
		if (this->buf[i] == needle.buf[cnt]) {
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

ssize_t OVERLOAD String_Find(String *this, String needle) {
	return String_FindRange(this, 0, this->len, needle);
}

ssize_t OVERLOAD String_Find(String *this, ssize_t offset, String needle) {
	return String_FindRange(this, offset, this->len - offset, needle);
}

ssize_t OVERLOAD String_Find(String *this, ssize_t offset, ssize_t length, String needle) {
	return String_FindRange(this, offset, length, needle);
}

ssize_t OVERLOAD String_Find(String *this, char c) {
	return String_FindRange(this, 0, this->len, c);
}

ssize_t OVERLOAD String_Find(String *this, ssize_t offset, char c) {
	return String_FindRange(this, offset, this->len - offset, c);
}

ssize_t OVERLOAD String_Find(String *this, ssize_t offset, ssize_t length, char c) {
	return String_FindRange(this, offset, length, c);
}

bool String_Contains(String *this, String needle) {
	return String_FindRange(this, 0, this->len, needle) != String_NotFound;
}

void String_TrimLeft(String *this) {
	size_t pos = 0;

	for (size_t i = 0; i < this->len; i++) {
		if (Char_IsWhitespace(this->buf[i])) {
			pos = i + 1;
		} else {
			break;
		}
	}

	String_Crop(this, pos, this->len);
}

void String_Trim(String *this) {
	size_t i, lpos = 0;

	for (i = 0; i < this->len; i++) {
		if (Char_IsWhitespace(this->buf[i])) {
			lpos = i + 1;
		} else {
			break;
		}
	}

	if (lpos == this->len) {
		this->len = 0;
	} else {
		size_t rpos = this->len - 1;

		for (i = rpos; i > 0; i--) {
			if (Char_IsWhitespace(this->buf[i])) {
				rpos = i - 1;
			} else {
				break;
			}
		}

		String_Crop(this, lpos, rpos - lpos + 1);
	}
}

String String_Format(String fmt, ...) {
	VarArg argptr;
	String param;
	size_t i = 0, length = 0;

	VarArg_Start(argptr, fmt);

	while (i < fmt.len) {
		if (
			fmt.buf[i] == '%' &&
			(i == 0 || (i > 0 && fmt.buf[i - 1] != '!'))
		) {
			param = VarArg_Get(argptr, String);
			length += param.len;
		} else {
			length++;
		}

		i++;
	}

	VarArg_End(argptr);

	String res = HeapString(length);

	VarArg_Start(argptr, fmt);

	i = 0;
	while (i < fmt.len) {
		if (
			fmt.buf[i] == '%' &&
			(i == 0 || (i > 0 && fmt.buf[i - 1] != '!'))
		) {
			param = VarArg_Get(argptr, String);

			if (param.len > 0) {
				Memory_Copy(res.buf + res.len, param.buf, param.len);
				res.len += param.len;
			}
		} else {
			res.buf[res.len] = fmt.buf[i];
			res.len++;
		}

		i++;
	}

	VarArg_End(argptr);

	return res;
}

ssize_t OVERLOAD String_Between(String *this, ssize_t offset, String left, String right, String *out) {
	ssize_t posLeft, posRight;

	if (offset < 0) {
		offset += this->len;
	}

	if ((posLeft = String_Find(this, offset, left)) == String_NotFound) {
		return String_NotFound;
	}

	posLeft += left.len;

	if ((posRight = String_Find(this, posLeft, right)) == String_NotFound) {
		return String_NotFound;
	}

	String_Copy(out, *this, posLeft, posRight - posLeft);

	return posRight + right.len;
}

ssize_t OVERLOAD String_Between(String *this, String left, String right, String *out) {
	return String_Between(this, 0, left, right, out);
}

String OVERLOAD String_Between(String *this, ssize_t offset, String left, String right) {
	String out = HeapString(0);
	String_Between(this, offset, left, right, &out);
	return out;
}

String OVERLOAD String_Between(String *this, String left, String right) {
	String out = HeapString(0);
	String_Between(this, 0, left, right, &out);
	return out;
}

bool String_Filter(String *this, String s1, String s2) {
	ssize_t left, right;

	if ((left = String_Find(this, s1)) == String_NotFound) {
		return false;
	}

	String out = HeapString(0);

	if (left > 0) {
		out = String_Slice(this, 0, left - 1);
	}

	left += s1.len;

	if ((right = String_Find(this, left, s2)) == String_NotFound) {
		String_Destroy(&out);
		return false;
	}

	String_Append(&out, *this, left, right - left);
	String_Append(&out, *this, right + s2.len);

	String_Copy(this, out);

	String_Destroy(&out);

	return true;
}

bool String_Outside(String *this, String left, String right) {
	ssize_t posLeft, posRight;

	if ((posLeft = String_Find(this, left)) == String_NotFound) {
		return false;
	}

	if ((posRight = String_Find(this, posLeft + left.len, right)) == String_NotFound) {
		return false;
	}

	String out = HeapString(posLeft + this->len - posRight - right.len);

	String_Append(&out, *this, 0, posLeft);
	String_Append(&out, *this, posRight + right.len);

	String_Copy(this, out);

	String_Destroy(&out);

	return true;
}

String String_Concat(String a, String b) {
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

bool String_Replace(String *this, String needle, String replacement) {
	ssize_t pos = String_Find(this, needle);

	if (pos == String_NotFound) {
		return false;
	}

	String out = HeapString(this->len - needle.len + replacement.len);

	String_Append(&out, *this, 0, pos);
	String_Append(&out, replacement);
	String_Append(&out, *this, pos + needle.len);

	String_Copy(this, out);

	String_Destroy(&out);

	return true;
}

void String_Print(String s) {
	if (s.buf != NULL) {
		write(STDOUT_FILENO, s.buf, s.len);
	}
}
