#include "String.h"

Exception_Define(String_BufferOverflowException);
Exception_Define(String_NotHeapAllocatedException);

static ExceptionManager *exc;

void String0(ExceptionManager *e) {
	exc = e;
}

void String_Destroy(String *this) {
	this->len = 0;
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

	this->buf = buf;
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

void String_Resize(String *this, size_t len) {
	if (!this->heap) {
		throw(exc, &String_NotHeapAllocatedException);
	}

	if (this->buf == NULL) {
		this->buf = Memory_Alloc(len);
	} else {
		this->buf = Memory_Realloc(this->buf, len);
	}

	this->size = len;

	if (this->len > len) {
		/* The string was shortened. */
		this->len = len;
	}
}

void String_Align(String *this, size_t len) {
	if (len > 0) {
		if (this->size == 0 || len > this->size) {
			String_Resize(this, len);
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

void String_Append(String *this, String s) {
	if (s.len == 0) {
		return;
	}

	String_Align(this, this->len + s.len);

	Memory_Copy(this->buf + this->len, s.buf, s.len);

	this->len = this->len + s.len;
}

void String_AppendChar(String *this, char c) {
	String_Align(this, this->len + 1);
	this->buf[this->len] = c;
	this->len++;
}

String String_Join(String *first, ...) {
	VarArg argptr;
	String *s;
	size_t len = first->len;

	VarArg_Start(argptr, first);

	while (true) {
		s = VarArg_Get(argptr, String *);

		if (s == NULL) {
			break;
		}

		len += s->len;
	}

	VarArg_End(argptr);

	String res = HeapString(len);

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

bool String_RangeEquals(String *this,
	ssize_t offset,
	String needle,
	ssize_t needleOffset)
{
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
		needle.len - needleOffset
	);
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

StringArray String_SplitChar(String *this, size_t offset, char c) {
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

static inline OVERLOAD ssize_t String_FindRange(String *this, size_t offset, size_t len, char c) {
	for (size_t i = offset; i < len; i++) {
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

static inline OVERLOAD ssize_t String_FindRange(String *this, size_t offset, size_t len, String needle) {
	size_t cntEqual = 0;

	while (offset < len) {
		if (this->buf[offset] == needle.buf[cntEqual]) {
			cntEqual++;

			if (cntEqual == needle.len) {
				return offset - needle.len + 1;
			}
		} else {
			if (cntEqual > 0) {
				cntEqual = 0;
				offset--;
			}
		}

		offset++;
	}

	return String_NotFound;
}

ssize_t OVERLOAD String_Find(String *this, String needle) {
	return String_FindRange(this, 0, this->len, needle);
}

ssize_t OVERLOAD String_Find(String *this, size_t offset, String needle) {
	return String_FindRange(this, offset, this->len - offset, needle);
}

ssize_t OVERLOAD String_Find(String *this, size_t offset, size_t len, String needle) {
	return String_FindRange(this, offset, len, needle);
}

ssize_t OVERLOAD String_Find(String *this, char c) {
	return String_FindRange(this, 0, this->len, c);
}

ssize_t OVERLOAD String_Find(String *this, size_t offset, char c) {
	return String_FindRange(this, offset, this->len - offset, c);
}

ssize_t OVERLOAD String_Find(String *this, size_t offset, size_t len, char c) {
	return String_FindRange(this, offset, len, c);
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
	size_t i = 0, len = 0;

	VarArg_Start(argptr, fmt);

	while (i < fmt.len) {
		if (
			fmt.buf[i] == '%' &&
			(i == 0 || (i > 0 && fmt.buf[i - 1] != '!'))
		) {
			param = VarArg_Get(argptr, String);
			len += param.len;
		} else {
			len++;
		}

		i++;
	}

	VarArg_End(argptr);

	String res = HeapString(len);

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

void String_Print(String s) {
	if (s.buf != NULL) {
		write(1, s.buf, s.len);
	}
}
