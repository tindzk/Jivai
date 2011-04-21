#import "String.h"

#define self String

sdef(size_t, GetSize, String s) {
	if (s.buf == NULL) {
		return 0;
	}

	return Pool_GetSize(Pool_GetInstance(), s.buf);
}

sdef(size_t, GetFree, String s) {
	if (s.buf == NULL) {
		return 0;
	}

	return Pool_GetSize(Pool_GetInstance(), s.buf) - s.len;
}

def(void, Destroy) {
	if (this->buf == (void *) 0xdeadbeef) {
		throw(DoubleFree);
	}

	/* Is the buffer safe to delete? */
	if (this->buf != NULL) {
		Pool_Free(Pool_GetInstance(), this->buf);
	}

	this->buf = (void *) 0xdeadbeef;
	this->len = 0;
}

/* Resizes the string's buffer to be `length' characters long. */
def(void, Resize, size_t length) {
	size_t realLength = (this->len > length)
		? length
		: this->len;

	if (this->buf != NULL) {
		this->buf = Pool_Realloc(Pool_GetInstance(), this->buf, length);
	} else {
		char *buf = Pool_Alloc(Pool_GetInstance(), length);

		if (realLength > 0) {
			Memory_Copy(buf, this->buf, realLength);
		}

		this->buf = buf;
	}

	if (this->len > length) {
		/* The string was shortened. */
		this->len = length;
	}
}

/* Ensures that the stringg is at least `length' characters long. */
def(void, Align, size_t length) {
	if (length == 0) {
		return;
	}

	if (this->buf == NULL) {
		call(Resize, length);
		return;
	}

	if (length < this->len) {
		return;
	}

	size_t size = Pool_GetSize(Pool_GetInstance(), this->buf);

	if (size == 0) {
		call(Resize, length);
	} else if (length > size) {
#if String_SmartAlign
		/* See also:
		 * http://stackoverflow.com/questions/2243366/how-to-implement-a-variable-length-string-y-in-c
		 */
		do {
			size <<= 1;
		} while (size < length);

		call(Resize, size);
#else
		call(Resize, length);
#endif
	}
}

sdef(self, Clone, RdString s) {
	self out = { .len = s.len };

	if (s.len > 0) {
		out.buf = Pool_Alloc(Pool_GetInstance(), s.len);
		Memory_Copy(out.buf, s.buf, s.len);
	}

	return out;
}

sdef(char, CharAt, RdString s, ssize_t offset) {
	if (offset < 0) {
		offset += s.len;
	}

	if ((size_t) offset > s.len) {
		throw(BufferOverflow);
	}

	return s.buf[offset];
}

overload sdef(RdString, Slice, RdString s, ssize_t offset, ssize_t length) {
	size_t right;

	if (offset < 0) {
		offset += s.len;
	}

	if (length < 0) {
		right = length + s.len;
	} else {
		right = length + offset;
	}

	if ((size_t) offset > right ||
		(size_t) offset > s.len ||
		right           > s.len)
	{
		throw(BufferOverflow);
	}

	return (RdString) {
		.len = right - offset,
		.buf = s.buf + offset
	};
}

overload sdef(void, Crop, self *dest, ssize_t offset, ssize_t length) {
	size_t right;

	if (offset < 0) {
		offset += dest->len;
	}

	if (length < 0) {
		right = length + dest->len;
	} else {
		right = length + offset;
	}

	if ((size_t) offset > right     ||
		(size_t) offset > dest->len ||
		right           > dest->len)
	{
		throw(BufferOverflow);
	}

	if (offset > 0 && right - offset > 0) {
		if (dest->buf != NULL) {
			/* Memory_Move is preferable because it also works with
			 * overlapping memory areas.
			 */

			Memory_Move(dest->buf,
				dest->buf + offset,
				right     - offset);
		} else {
			char *buf = Pool_Alloc(Pool_GetInstance(), right - offset);

			Memory_Copy(buf,
				dest->buf + offset,
				right     - offset);

			dest->buf = buf;
		}
	}

	dest->len = right - offset;
}

def(void, Delete, ssize_t offset, ssize_t length) {
	size_t from;

	if (offset < 0) {
		offset += this->len;
	}

	from = offset + length;

	if ((size_t) offset > from      ||
		(size_t) offset > this->len ||
		from            > this->len)
	{
		throw(BufferOverflow);
	}

	Memory_Move(
		this->buf + offset,
		this->buf + from,
		this->len - from);

	this->len = this->len - length;
}

overload sdef(void, Prepend, self *dest, RdString s) {
	self tmp = scall(Concat, s, dest->rd);
	scall(Copy, dest, tmp.rd);
	scall(Destroy, &tmp);
}

overload sdef(void, Prepend, self *dest, char c) {
	self tmp = String_New(dest->len + 1);
	scall(Append, &tmp, c);
	scall(Append, &tmp, dest->rd);
	scall(Copy, dest, tmp.rd);
	scall(Destroy, &tmp);
}

def(void, Assign, String src) {
	call(Destroy);

	this->buf = src.buf;
	this->len = src.len;
}

def(void, Copy, RdString src) {
	if (src.len > 0) {
		call(Align, src.len);
		Memory_Move(this->buf, src.buf, src.len);
	}

	this->len = src.len;
}

overload sdef(void, Append, self *dest, RdString s) {
	if (s.len == 0) {
		return;
	}

	/* Resize the buffer if necessary. */
	scall(Align, dest, dest->len + s.len);

	/* Append s. */
	Memory_Copy(dest->buf + dest->len, s.buf, s.len);
	dest->len += s.len;
}

overload sdef(void, Append, self *dest, char c) {
	scall(Align, dest, dest->len + 1);
	dest->buf[dest->len] = c;
	dest->len++;
}

overload sdef(void, Append, self *dest, FmtString s) {
#if String_FmtChecks
	s.val++;
#endif

	RdString *val = s.val;

	size_t len = 0;

	fwd(i, s.fmt.len) {
		if (s.fmt.buf[i] == '%') {
			if (i == 0 || s.fmt.buf[i - 1] != '!') {
#if String_FmtChecks
				if (s.val->len == (size_t) -1) {
					throw(ElementMismatch);
				}
#endif

				len += s.val->len;
				s.val++;
			}
		} else {
			len++;
		}
	}

	scall(Align, dest, len);

	fwd(i, s.fmt.len) {
		if (i + 1 < s.fmt.len && s.fmt.buf[i] == '!' && s.fmt.buf[i + 1] == '%') {
			dest->buf[dest->len] = '%';
			dest->len++, i++;
		} else if (s.fmt.buf[i] == '%') {
			if (val->len > 0) {
				Memory_Copy(dest->buf + dest->len, val->buf, val->len);
				dest->len += val->len;
			}

			val++;
		} else {
			dest->buf[dest->len] = s.fmt.buf[i];
			dest->len++;
		}
	}
}

sdef(bool, RangeEquals, RdString s, ssize_t offset, RdString needle, ssize_t needleOffset) {
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

def(void, ToLower) {
	fwd(i, this->len) {
		this->buf[i] = (char) Char_ToLower(this->buf[i]);
	}
}

def(void, ToUpper) {
	fwd(i, this->len) {
		this->buf[i] = (char) Char_ToUpper(this->buf[i]);
	}
}

overload sdef(bool, Split, RdString s, char c, RdString *res) {
	size_t offset = (res->buf != NULL)
		? res->buf - s.buf + res->len + 1
		: 0;

	if (s.len == 0 || offset > s.len) {
		return false;
	}

	for (size_t pos = offset; pos <= s.len; pos++) {
		if (pos == s.len || s.buf[pos] == c) {
			res->buf = s.buf + offset;
			res->len = pos   - offset;

			break;
		}
	}

	return true;
}

overload sdef(bool, Split, RdString s, RdString needle, RdString *res) {
	size_t offset = (res->buf != NULL)
		? res->buf - s.buf + res->len + needle.len
		: 0;

	if (s.len == 0 || offset > s.len) {
		return false;
	}

	size_t cnt = 0;

	for (size_t pos = offset; pos <= s.len; pos++) {
		if (pos == s.len) {
			res->buf = s.buf + offset;
			res->len = pos   - offset;

			break;
		} else if (s.buf[pos] == needle.buf[cnt]) {
			cnt++;

			if (cnt == needle.len) {
				res->buf = s.buf + offset;
				res->len = pos   - offset - 1;

				break;
			}
		} else if (cnt > 0) {
			cnt = 0;
			pos--;
		}
	}

	return true;
}

overload sdef(RdStringArray *, Split, RdString s, char c) {
	size_t chunks = 1;
	fwd(i, s.len) {
		if (s.buf[i] == c) {
			chunks++;
		}
	}

	RdStringArray *res = RdStringArray_New(chunks);

	RdString elem = $("");
	while (String_Split(s, c, &elem)) {
		res->buf[res->len] = elem;
		res->len++;
	}

	return res;
}

sdef(bool, Parse, RdString pattern, RdString subject, ...) {
	size_t subj =  0;
	ssize_t ofs = -1;

	VarArg argptr;
	VarArg_Start(argptr, subject);

	fwd(i, pattern.len) {
		if (pattern.buf[i] == '%') {
			ofs = subj;
		} else {
			if (ofs != -1) {
				RdString *value = VarArg_Get(argptr, RdString *);

				value->buf = subject.buf + ofs;
				value->len = subject.len - ofs;

				fwd(j, value->len) {
					if (value->buf[j] == pattern.buf[i]) {
						value->len = j;
						subj = j + ofs;
						break;
					}
				}

				bool found = (value->len != subject.len - ofs);

				if (!found) {
					VarArg_End(argptr);
					return false;
				}

				ofs = -1;
			}

			if (subject.buf[subj] != pattern.buf[i]) {
				VarArg_End(argptr);
				return false;
			}

			subj++;
		}
	}

	if (ofs != -1) {
		RdString *value = VarArg_Get(argptr, RdString *);

		value->buf = subject.buf + ofs;
		value->len = subject.len - ofs;
	}

	VarArg_End(argptr);

	return true;
}

overload sdef(ssize_t, Find, RdString s, ssize_t offset, ssize_t length, char c) {
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

overload sdef(ssize_t, ReverseFind, RdString s, ssize_t offset, char c) {
	if (s.len == 0) {
		return ref(NotFound);
	}

	if (offset < 0) {
		offset += s.len - 1;
	}

	if ((size_t) offset > s.len) {
		throw(BufferOverflow);
	}

	bwd(i, offset + 1) {
		if (s.buf[i] == c) {
			return i;
		}
	}

	return ref(NotFound);
}

overload sdef(ssize_t, ReverseFind, RdString s, ssize_t offset, RdString needle) {
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

	bwd(i, offset + 1) {
		if (s.buf[i] == needle.buf[needle.len - cnt - 1]) {
			cnt++;

			if (cnt == needle.len) {
				return i;
			}
		} else if (cnt > 0) {
			cnt = 0;
			i++;
		}
	}

	return ref(NotFound);
}

overload sdef(ssize_t, Find, RdString s, ssize_t offset, ssize_t length, RdString needle) {
	size_t right;

	if (offset < 0) {
		offset += s.len;
	}

	if (length < 0) {
		right = length + s.len;
	} else {
		right = length + offset;
	}

	if ((size_t) offset > s.len ||
		(size_t) right  > s.len)
	{
		throw(BufferOverflow);
	}

	size_t cnt = 0;

	for (size_t i = offset; i < right; i++) {
		if (s.buf[i] == needle.buf[cnt]) {
			cnt++;

			if (cnt == needle.len) {
				return i - needle.len + 1;
			}
		} else if (cnt > 0) {
			cnt = 0;
			i--;
		}
	}

	return ref(NotFound);
}

overload sdef(RdString, Trim, RdString s, short type) {
	if (BitMask_Has(type, ref(TrimLeft))) {
		while (s.len > 0 && Char_IsSpace(*s.buf)) {
			s.buf++;
			s.len--;
		}
	}

	if (BitMask_Has(type, ref(TrimRight))) {
		while (s.len > 0 && Char_IsSpace(s.buf[s.len - 1])) {
			s.len--;
		}
	}

	return s;
}

overload sdef(ssize_t, Between, RdString s, ssize_t offset, RdString left, RdString right, bool leftAligned, RdString *out) {
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

	*out = (RdString) {
		.buf = s.buf + posLeft,
		.len = posRight - posLeft
	};

	return posRight + right.len;
}

sdef(RdString, Cut, RdString s, RdString left, RdString right) {
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

def(bool, Filter, RdString s1, RdString s2) {
	ssize_t left, right;

	if ((left = scall(Find, this->rd, s1)) == ref(NotFound)) {
		return false;
	}

	String out = String_New(0);

	if (left > 0) {
		out = String_Clone(String_Slice(this->rd, 0, left - 1));
	}

	left += s1.len;

	if ((right = scall(Find, this->rd, left, s2)) == ref(NotFound)) {
		return false;
	}

	scall(Append, &out, String_Slice(this->rd, left, right - left));
	scall(Append, &out, String_Slice(this->rd, right + s2.len));

	scall(Copy, this, out.rd);

	scall(Destroy, &out);

	return true;
}

def(bool, Outside, RdString left, RdString right) {
	ssize_t posLeft, posRight;

	if ((posLeft = scall(Find, this->rd, left)) == ref(NotFound)) {
		return false;
	}

	if ((posRight = scall(Find, this->rd, posLeft + left.len, right)) == ref(NotFound)) {
		return false;
	}

	self out = String_New(posLeft + this->len - posRight - right.len);

	scall(Append, &out, String_Slice(this->rd, 0, posLeft));
	scall(Append, &out, String_Slice(this->rd, posRight + right.len));

	scall(Copy, this, out.rd);

	scall(Destroy, &out);

	return true;
}

overload sdef(self, Concat, RdString a, RdString b) {
	self res = String_New(a.len + b.len);

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

overload sdef(self, Concat, RdString s, char c) {
	self res = String_New(s.len + 1);

	if (s.len > 0) {
		Memory_Copy(res.buf, s.buf, s.len);
	}

	res.buf[s.len] = c;

	res.len = s.len + 1;

	return res;
}

overload sdef(bool, Replace, self *dest, ssize_t offset, RdString needle, RdString replacement) {
	if (offset < 0) {
		offset += dest->len;
	}

	ssize_t pos = scall(Find, dest->rd, offset, needle);

	if (pos == ref(NotFound)) {
		return false;
	}

	ssize_t len = dest->len - needle.len + replacement.len;

	if (len < 0) {
		len = replacement.len;
	}

	self out = String_New(len);

	scall(Append, &out, String_Slice(dest->rd, 0, pos));
	scall(Append, &out, replacement);
	scall(Append, &out, String_Slice(dest->rd, pos + needle.len));

	scall(Copy, dest, out.rd);

	scall(Destroy, &out);

	return true;
}

overload sdef(bool, ReplaceAll, self *dest, ssize_t offset, RdString needle, RdString replacement) {
	if (offset < 0) {
		offset += dest->len;
	}

	ssize_t len = dest->len - needle.len + replacement.len;

	if (len < 0) {
		len = replacement.len;
	}

	/* Approximation for one occurence. */
	self out = String_New(len);

	size_t cnt     = 0;
	size_t lastPos = 0;

	for (size_t i = offset; i < dest->len; i++) {
		if (dest->buf[i] == needle.buf[cnt]) {
			cnt++;

			if (cnt == needle.len) {
				size_t cur = i - needle.len + 1;

				scall(Append, &out, String_Slice(dest->rd, lastPos, cur - lastPos));
				scall(Append, &out, replacement);

				lastPos = i + 1;
				cnt = 0;
			}
		} else if (cnt > 0) {
			cnt = 0;
			i--;
		}
	}

	scall(Append, &out, String_Slice(dest->rd, lastPos));

	scall(Copy, dest, out.rd);

	scall(Destroy, &out);

	return lastPos != 0;
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

sdef(short, CompareRight, RdString a, RdString b) {
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

sdef(short, CompareLeft, RdString a, RdString b) {
	/* Compare two left-aligned numbers: the first to have a
	 * different value wins.
	 */

	for (size_t i = 0; i < a.len && i < b.len; i++) {
		if (!Char_IsDigit(a.buf[i]) && !Char_IsDigit(b.buf[i])) {
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

overload sdef(short, NaturalCompare, RdString a, RdString b, bool foldcase, bool skipSpaces, bool skipZeros) {
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

			RdString __a = scall(Slice, a, ai);
			RdString __b = scall(Slice, b, bi);

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

#undef self

#define self RdStringArray

def(ssize_t, Find, RdString needle) {
	fwd(i, this->len) {
		if (String_Equals(this->buf[i], needle)) {
			return i;
		}
	}

	return -1;
}

def(String, Join, RdString separator) {
	size_t len = 0;

	fwd(i, this->len) {
		len += this->buf[i].len;

		if (i != this->len - 1) {
			len += separator.len;
		}
	}

	String res = String_New(len);

	fwd(i, this->len) {
		String_Append(&res, this->buf[i]);

		if (i != this->len - 1) {
			String_Append(&res, separator);
		}
	}

	return res;
}

def(bool, Contains, RdString needle) {
	return call(Find, needle) != -1;
}

#undef self

#define self StringArray

def(ssize_t, Find, RdString needle) {
	fwd(i, this->len) {
		if (String_Equals(this->buf[i].rd, needle)) {
			return i;
		}
	}

	return -1;
}

def(bool, Contains, RdString needle) {
	return call(Find, needle) != -1;
}

def(void, Destroy) {
	fwd(i, this->len) {
		String_Destroy(&this->buf[i]);
	}
}

#undef self

#define self CarrierStringArray

def(void, Destroy) {
	fwd(i, this->len) {
		CarrierString_Destroy(&this->buf[i]);
	}
}
