#import "String.h"

#define self String

inline self HeapString(size_t len) {
	return (self) {
		.len  = 0,
		.size = len,
		.buf  = (len > 0)
			? Memory_Alloc(len)
			: NULL,
		.mutable = true
	};
}

inline self BufString(char *buf, size_t len) {
	return (self) {
		.len     = len,
		.size    = 0,
		.buf     = buf,
		.mutable = false
	};
}

def(void, Destroy) {
	if (!this->mutable) {
		throw(NotMutable);
	}

	this->len = 0;
	this->mutable = false;

	if (this->size > 0) {
		this->size = 0;
		Memory_Free(this->buf);
	}
}

inline sdef(self, FromNul, char *s) {
	return BufString(s,
		(s != NULL)
			? strlen(s)
			: 0);
}

inline sdef(char *, ToNulBuf, self s, char *buf) {
	if (s.len > 0) {
		Memory_Copy(buf, s.buf, s.len);
	}

	buf[s.len] = '\0';

	return buf;
}

inline sdef(char *, ToNulHeap, self s) {
	return scall(ToNulBuf, s, Memory_Alloc(s.len + 1));
}

inline sdef(self, Disown, self s) {
	s.mutable = false;
	return s;
}

def(void, Resize, size_t length) {
	if (!this->mutable) {
		throw(NotMutable);
	}

	if (length == 0) {
		if (this->size > 0) {
			Memory_Free(this->buf);
		}
	} else {
		if (this->size > 0) {
			this->buf = Memory_Realloc(this->buf, length);
		} else {
			char *buf = Memory_Alloc(length);

			if (this->len > 0) {
				Memory_Copy(buf, this->buf, this->len);
			}

			this->buf = buf;
		}
	}

	this->size = length;

	if (this->len > length) {
		/* The string was shortened. */
		this->len = length;
	}
}

def(void, Align, size_t length) {
	if (length > 0) {
		if (this->size == 0) {
			call(Resize, length);
		} else if (length > this->size) {
#if String_SmartAlign
			/* See also:
			 * http://stackoverflow.com/questions/2243366/how-to-implement-a-variable-length-string-y-in-c
			 */
			size_t size = this->size;

			do {
				size <<= 1;
			} while (size < length);

			call(Resize, size);
#else
			call(Resize, length);
#endif
		}
	}
}

sdef(void, Copy, self *dest, self src) {
	if (!dest->mutable) {
		throw(NotMutable);
	}

	if (dest->size == 0) {
		if (src.size == 0) {
			dest->buf = src.buf;
			goto out;
		}
	}

	if (src.len > 0) {
		if (dest->size == 0) {
			dest->buf = Memory_Alloc(src.len);
		} else if (src.len > dest->size) {
			dest->buf = Memory_Realloc(dest->buf, src.len);
		}

		dest->size = src.len;
		Memory_Copy(dest->buf, src.buf, src.len);
	}

out:
	dest->len = src.len;
}

sdef(self, Clone, self s) {
	self out = {
		.buf  = NULL,
		.len  = 0,
		.size = 0,
		.mutable = true
	};

	if (s.size == 0) {
		/* `s' is a rodata string, probably created with $(...). */
		out.buf = s.buf;
		out.len = s.len;
	} else if (s.len > 0) {
		out.buf = Memory_Alloc(s.len);
		out.size = s.len;

		Memory_Copy(out.buf, s.buf, s.len);
		out.len = s.len;
	}

	return out;
}

sdef(char *, CloneBuf, self s, char *buf) {
	if (s.len > 0) {
		Memory_Copy(buf, s.buf, s.len);
	}

	return buf;
}

sdef(char, CharAt, self s, ssize_t offset) {
	if (offset < 0) {
		offset += s.len;
	}

	if ((size_t) offset > s.len) {
		throw(BufferOverflow);
	}

	return s.buf[offset];
}

overload sdef(self, Slice, self s, ssize_t offset, ssize_t length) {
	self out;
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
		throw(BufferOverflow);
	}

	out.len     = right - offset;
	out.size    = out.len;
	out.buf     = s.buf + offset;
	out.mutable = false;

	return out;
}

inline overload sdef(self, Slice, self s, ssize_t offset) {
	if (offset < 0) {
		offset += s.len;
	}

	return scall(Slice, s, offset, s.len - offset);
}

overload sdef(void, Crop, self *dest, ssize_t offset, ssize_t length) {
	if (!dest->mutable) {
		throw(NotMutable);
	}

	size_t right;

	if (offset < 0) {
		offset += dest->len;
	}

	if (length < 0) {
		right = length + dest->len;
	} else {
		right = length + offset;
	}

	if ((size_t) offset > right
	 || (size_t) offset > dest->len
	 || right           > dest->len) {
		throw(BufferOverflow);
	}

	if (offset > 0) {
		if (right - offset > 0) {
			/* Memory_Move is preferable because it also works with
			 * overlapping memory areas.
			 */

			Memory_Move(dest->buf,
				dest->buf + offset,
				right     - offset);
		}

		dest->len = right - offset;
	} else {
		dest->len = right;
	}
}

inline overload sdef(void, Crop, self *dest, ssize_t offset) {
	if (offset < 0) {
		offset += dest->len;
	}

	scall(Crop, dest, offset, dest->len - offset);
}

def(void, Delete, ssize_t offset, ssize_t length) {
	if (!this->mutable) {
		throw(NotMutable);
	}

	size_t from;

	if (offset < 0) {
		offset += this->len;
	}

	from = offset + length;

	if ((size_t) offset > from
	 || (size_t) offset > this->len
	 || from            > this->len) {
		throw(BufferOverflow);
	}

	Memory_Move(
		this->buf + offset,
		this->buf + from,
		this->len - from);

	this->len = this->len - length;
}

overload sdef(void, Prepend, self *dest, self s) {
	self tmp = scall(Concat, s, *dest);
	scall(Copy, dest, tmp);
	scall(Destroy, &tmp);
}

overload sdef(void, Prepend, self *dest, char c) {
	self tmp = HeapString(dest->len + 1);
	scall(Append, &tmp, c);
	scall(Append, &tmp, *dest);
	scall(Copy, dest, tmp);
	scall(Destroy, &tmp);
}

overload sdef(void, Append, self *dest, self s) {
	if (!dest->mutable) {
		throw(NotMutable);
	}

	if (s.len == 0) {
		return;
	}

	if (s.size == 0) {
		if (dest->size == 0) {
			dest->buf = s.buf;
			dest->len = s.len;

			return;
		}
	}

	scall(Align, dest, dest->len + s.len);

	Memory_Copy(dest->buf + dest->len, s.buf, s.len);

	dest->len = dest->len + s.len;
}

overload sdef(void, Append, self *dest, char c) {
	scall(Align, dest, dest->len + 1);
	dest->buf[dest->len] = c;
	dest->len++;
}

sdef(self, Join, self *first, ...) {
	self *s;
	VarArg argptr;
	size_t length = first->len;

	VarArg_Start(argptr, first);

	while (true) {
		s = VarArg_Get(argptr, self *);

		if (s == NULL) {
			break;
		}

		length += s->len;
	}

	VarArg_End(argptr);

	self res = HeapString(length);

	VarArg_Start(argptr, first);

	s = first;

	while (true) {
		if (s->len > 0) {
			Memory_Copy(res.buf + res.len, s->buf, s->len);
			res.len += s->len;
		}

		s = VarArg_Get(argptr, self *);

		if (s == NULL) {
			break;
		}
	}

	VarArg_End(argptr);

	return res;
}

inline sdef(bool, Equals, self s, self needle) {
	return s.len == needle.len && Memory_Equals(s.buf, needle.buf, s.len);
}

sdef(bool, RangeEquals, self s, ssize_t offset, self needle, ssize_t needleOffset) {
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

inline sdef(bool, BeginsWith, self s, self needle) {
	return scall(RangeEquals, s, 0, needle, 0);
}

inline sdef(bool, EndsWith, self s, self needle) {
	return scall(RangeEquals, s, s.len - needle.len, needle, 0);
}

def(void, ToLower) {
	if (!this->mutable) {
		throw(NotMutable);
	}

	for (size_t i = 0; i < this->len; i++) {
		this->buf[i] = (char) Char_ToLower(this->buf[i]);
	}
}

def(void, ToUpper) {
	if (!this->mutable) {
		throw(NotMutable);
	}

	for (size_t i = 0; i < this->len; i++) {
		this->buf[i] = (char) Char_ToUpper(this->buf[i]);
	}
}

overload sdef(StringArray *, Split, self s, size_t offset, char c) {
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
			res->buf[res->len] = scall(Slice, s, left, right - left);
			res->len++;
			left = right + 1;
		}
	}

	res->buf[res->len] = scall(Slice, s, left, right - left);
	res->len++;

	return res;
}

inline overload sdef(StringArray *, Split, self s, char c) {
	return scall(Split, s, 0, c);
}

overload sdef(ssize_t, Find, self s, ssize_t offset, ssize_t length, char c) {
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

	return ref(NotFound);
}

overload sdef(ssize_t, ReverseFind, self s, ssize_t offset, char c) {
	if (s.len == 0) {
		return ref(NotFound);
	}

	if (offset < 0) {
		offset += s.len - 1;
	}

	if ((size_t) offset > s.len) {
		throw(BufferOverflow);
	}

	for (ssize_t i = offset; i >= 0; i--) {
		if (s.buf[i] == c) {
			return i;
		}
	}

	return ref(NotFound);
}

inline overload sdef(ssize_t, ReverseFind, self s, char c) {
	return scall(ReverseFind, s, s.len - 1, c);
}

overload sdef(ssize_t, ReverseFind, self s, ssize_t offset, self needle) {
	if (s.len == 0) {
		return ref(NotFound);
	}

	if (offset < 0) {
		offset += s.len - 1;
	}

	if ((size_t) offset > s.len) {
		throw(BufferOverflow);
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

	return ref(NotFound);
}

inline overload sdef(ssize_t, ReverseFind, self s, self needle) {
	return scall(ReverseFind, s, s.len - 1, needle);
}

overload sdef(ssize_t, Find, self s, ssize_t offset, ssize_t length, self needle) {
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
		throw(BufferOverflow);
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

	return ref(NotFound);
}

inline overload sdef(ssize_t, Find, self s, self needle) {
	return scall(Find, s, 0, s.len, needle);
}

inline overload sdef(ssize_t, Find, self s, ssize_t offset, self needle) {
	if (offset < 0) {
		offset += s.len;
	}

	return scall(Find, s, offset, s.len - offset, needle);
}

inline overload sdef(ssize_t, Find, self s, char c) {
	return scall(Find, s, 0, s.len, c);
}

inline overload sdef(ssize_t, Find, self s, ssize_t offset, char c) {
	if (offset < 0) {
		offset += s.len;
	}

	return scall(Find, s, offset, s.len - offset, c);
}

overload sdef(bool, Contains, self s, self needle) {
	return scall(Find, s, 0, s.len, needle) != ref(NotFound);
}

overload sdef(bool, Contains, self s, char needle) {
	return scall(Find, s, 0, s.len, needle) != ref(NotFound);
}

overload sdef(void, Trim, self *dest, short type) {
	size_t i, lpos = 0;

	if (BitMask_Has(type, ref(TrimLeft))) {
		for (i = 0; i < dest->len; i++) {
			if (Char_IsSpace(dest->buf[i])) {
				lpos = i + 1;
			} else {
				break;
			}
		}
	}

	if (lpos == dest->len) {
		dest->len = 0;
	} else {
		size_t rpos = dest->len;

		if (BitMask_Has(type, ref(TrimRight))) {
			for (i = rpos; i > 0; i--) {
				if (Char_IsSpace(dest->buf[i - 1])) {
					rpos = i - 1;
				} else {
					break;
				}
			}
		}

		scall(Crop, dest, lpos, rpos - lpos);
	}
}

inline overload sdef(void, Trim, self *dest) {
	scall(Trim, dest,
		ref(TrimLeft) |
		ref(TrimRight));
}

overload sdef(self, Trim, self s, short type) {
	size_t i, lpos = 0;

	if (BitMask_Has(type, ref(TrimLeft))) {
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

		if (BitMask_Has(type, ref(TrimRight))) {
			for (i = rpos; i > 0; i--) {
				if (Char_IsSpace(s.buf[i - 1])) {
					rpos = i - 1;
				} else {
					break;
				}
			}
		}

		return scall(Slice, s, lpos, rpos - lpos);
	}

	return s;
}

inline overload sdef(self, Trim, self s) {
	return scall(Trim, s,
		ref(TrimLeft) |
		ref(TrimRight));
}

sdef(self, Format, self fmt, ...) {
	size_t length = 0;
	VarArg argptr;
	self param;

	VarArg_Start(argptr, fmt);

	for (size_t i = 0; i < fmt.len; i++) {
		if (fmt.buf[i] == '%') {
			if (i == 0 || fmt.buf[i - 1] != '!') {
				length += VarArg_Get(argptr, self).len;
			}
		} else {
			length++;
		}
	}

	VarArg_End(argptr);

	self res = HeapString(length);

	VarArg_Start(argptr, fmt);

	for (size_t i = 0; i < fmt.len; i++) {
		if (i + 1 != fmt.len && fmt.buf[i] == '!' && fmt.buf[i + 1] == '%') {
			res.buf[res.len] = '%';
			res.len++;
		} else if (fmt.buf[i] == '%') {
			param = VarArg_Get(argptr, self);

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

overload sdef(ssize_t, Between, self s, ssize_t offset, self left, self right, bool leftAligned, self *out) {
	ssize_t posLeft, posRight;

	if (offset < 0) {
		offset += s.len;
	}

	if (leftAligned) {
		if ((posLeft = scall(Find, s, offset, left)) == ref(NotFound)) {
			return ref(NotFound);
		}

		posLeft += left.len;

		if ((posRight = scall(Find, s, posLeft + 1, right)) == ref(NotFound)) {
			return ref(NotFound);
		}
	} else {
		if ((posRight = scall(Find, s, offset, right)) == ref(NotFound)) {
			return ref(NotFound);
		}

		if (posRight > 0) {
			if ((posLeft = scall(ReverseFind, s, posRight - 1, left)) == ref(NotFound)) {
				return ref(NotFound);
			}

			posLeft += left.len;
		} else {
			return ref(NotFound);
		}
	}

	*out = BufString(s.buf + posLeft, posRight - posLeft);

	return posRight + right.len;
}

inline overload sdef(ssize_t, Between, self s, self left, self right, self *out) {
	return scall(Between, s, 0, left, right, true, out);
}

inline overload sdef(ssize_t, Between, self s, ssize_t offset, self left, self right, self *out) {
	return scall(Between, s, offset, left, right, true, out);
}

inline overload sdef(self, Between, self s, ssize_t offset, self left, self right, bool leftAligned) {
	self out = NullString;
	scall(Between, s, offset, left, right, leftAligned, &out);
	return out;
}

inline overload sdef(self, Between, self s, ssize_t offset, self left, self right) {
	self out = NullString;
	scall(Between, s, offset, left, right, &out);
	return out;
}

inline overload sdef(self, Between, self s, self left, self right, bool leftAligned) {
	self out = NullString;
	scall(Between, s, 0, left, right, leftAligned, &out);
	return out;
}

inline overload sdef(self, Between, self s, self left, self right) {
	self out = NullString;
	scall(Between, s, 0, left, right, true, &out);
	return out;
}

sdef(self, Cut, self s, self left, self right) {
	ssize_t posLeft = scall(Find, s, left);

	if (posLeft == ref(NotFound)) {
		return $("");
	}

	ssize_t posRight = scall(Find, s, posLeft + left.len, right);

	if (posRight == ref(NotFound)) {
		return $("");
	}

	return scall(Slice, s, posLeft, posRight - posLeft);
}

def(bool, Filter, self s1, self s2) {
	ssize_t left, right;

	if ((left = scall(Find, *this, s1)) == ref(NotFound)) {
		return false;
	}

	self out = HeapString(0);

	if (left > 0) {
		out = scall(Clone, String_Slice(*this, 0, left - 1));
	}

	left += s1.len;

	if ((right = scall(Find, *this, left, s2)) == ref(NotFound)) {
		scall(Destroy, &out);
		return false;
	}

	scall(Append, &out, String_Slice(*this, left, right - left));
	scall(Append, &out, String_Slice(*this, right + s2.len));

	scall(Copy, this, out);

	scall(Destroy, &out);

	return true;
}

def(bool, Outside, self left, self right) {
	ssize_t posLeft, posRight;

	if ((posLeft = scall(Find, *this, left)) == ref(NotFound)) {
		return false;
	}

	if ((posRight = scall(Find, *this, posLeft + left.len, right)) == ref(NotFound)) {
		return false;
	}

	self out = HeapString(posLeft + this->len - posRight - right.len);

	scall(Append, &out, String_Slice(*this, 0, posLeft));
	scall(Append, &out, String_Slice(*this, posRight + right.len));

	scall(Copy, this, out);

	scall(Destroy, &out);

	return true;
}

overload sdef(self, Concat, self a, self b) {
	self res = HeapString(a.len + b.len);

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

overload sdef(self, Concat, self s, char c) {
	self res = HeapString(s.len + 1);

	if (s.len > 0) {
		Memory_Copy(res.buf, s.buf, s.len);
	}

	res.buf[s.len] = c;

	res.len = s.len + 1;

	return res;
}

overload sdef(bool, Replace, self *dest, ssize_t offset, self needle, self replacement) {
	if (offset < 0) {
		offset += dest->len;
	}

	ssize_t pos = scall(Find, *dest, offset, needle);

	if (pos == ref(NotFound)) {
		return false;
	}

	ssize_t len = dest->len - needle.len + replacement.len;

	if (len < 0) {
		len = replacement.len;
	}

	self out = HeapString(len);

	scall(Append, &out, String_Slice(*dest, 0, pos));
	scall(Append, &out, replacement);
	scall(Append, &out, String_Slice(*dest, pos + needle.len));

	scall(Copy, dest, out);

	scall(Destroy, &out);

	return true;
}

inline overload sdef(bool, Replace, self *dest, self needle, self replacement) {
	return scall(Replace, dest, 0, needle, replacement);
}

inline overload sdef(self, Replace, self s, self needle, self replacement) {
	self tmp = s;
	scall(Replace, &tmp, 0, needle, replacement);
	return tmp;
}

overload sdef(bool, ReplaceAll, self *dest, ssize_t offset, self needle, self replacement) {
	if (offset < 0) {
		offset += dest->len;
	}

	ssize_t len = dest->len - needle.len + replacement.len;

	if (len < 0) {
		len = replacement.len;
	}

	/* Approximation for one occurence. */
	self out = HeapString(len);

	size_t cnt     = 0;
	size_t lastPos = 0;

	for (size_t i = offset; i < dest->len; i++) {
		if (dest->buf[i] == needle.buf[cnt]) {
			cnt++;

			if (cnt == needle.len) {
				size_t cur = i - needle.len + 1;

				scall(Append, &out, String_Slice(*dest, lastPos, cur - lastPos));
				scall(Append, &out, replacement);

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

	scall(Append, &out, String_Slice(*dest, lastPos));

	scall(Copy, dest, out);

	scall(Destroy, &out);

	return lastPos != 0;
}

inline overload sdef(bool, ReplaceAll, self *dest, self needle, self replacement) {
	return scall(ReplaceAll, dest, 0, needle, replacement);
}

inline overload sdef(self, ReplaceAll, self s, self needle, self replacement) {
	self tmp = scall(Clone, s);
	scall(ReplaceAll, &tmp, 0, needle, replacement);
	return tmp;
}

def(self, Consume, size_t n) {
	self res = scall(Clone, String_Slice(*this, 0, n));
	scall(Crop, this, n);
	return res;
}

overload sdef(void, Print, self s, bool err) {
	if (s.buf != NULL) {
		Kernel_write(err ? FileNo_StdErr : FileNo_StdOut, s.buf, s.len);
	}
}

inline overload sdef(void, Print, self s) {
	scall(Print, s, false);
}

/*
 * CompareRight(), CompareLeft() and NaturalCompare() are based upon
 * Martin Pool's `strnatcmp' library:
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

sdef(short, CompareRight, self a, self b) {
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

sdef(short, CompareLeft, self a, self b) {
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

overload sdef(short, NaturalCompare, self a, self b, bool foldcase, bool skipSpaces, bool skipZeros) {
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

			self __a = scall(Slice, a, ai);
			self __b = scall(Slice, b, bi);

			if (!skipZeros) {
				/* Is fractional? */
				if (a.buf[ai] == '0' || b.buf[bi] == '0') {
					result = scall(CompareLeft, __a, __b);

					if (result != 0) {
						return result;
					}
				}
			}

			result = scall(CompareRight, __a, __b);

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

inline overload sdef(short, NaturalCompare, self a, self b) {
	return scall(NaturalCompare, a, b, true, true, true);
}

#undef self
#define self StringArray

def(ssize_t, Find, String needle) {
	for (size_t i = 0; i < this->len; i++) {
		if (String_Equals(this->buf[i], needle)) {
			return i;
		}
	}

	return -1;
}

def(String, Join, String separator) {
	size_t len = 0;

	for (size_t i = 0; i < this->len; i++) {
		len += this->buf[i].len;

		if (i != this->len - 1) {
			len += separator.len;
		}
	}

	String res = HeapString(len);

	for (size_t i = 0; i < this->len; i++) {
		String_Append(&res, this->buf[i]);

		if (i != this->len - 1) {
			String_Append(&res, separator);
		}
	}

	return res;
}

def(bool, Contains, String needle) {
	return StringArray_Find(this, needle) != -1;
}

def(void, Destroy) {
	for (size_t i = 0; i < this->len; i++) {
		String_Destroy(&this->buf[i]);
	}

	this->len = 0;
}

def(void, ToHeap) {
	for (size_t i = 0; i < this->len; i++) {
		if (!this->buf[i].mutable) {
			this->buf[i] = String_Clone(this->buf[i]);
		}
	}
}
