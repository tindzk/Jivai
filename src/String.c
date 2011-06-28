#import "String.h"
#import "Exception.h"

#define self String

sdef(size_t, GetSize, String s) {
	if (s.buf == NULL) {
		return 0;
	}

	return Memory_GetSize(s.buf);
}

sdef(size_t, GetFree, String s) {
	if (s.buf == NULL) {
		return 0;
	}

	return Memory_GetSize(s.buf) - s.len;
}

def(void, Destroy) {
	if (this->buf == (void *) 0xdeadbeef) {
		throw(DoubleFree);
	}

	/* Is the buffer safe to delete? */
	if (this->buf != NULL) {
		Memory_Destroy(this->buf);
	}

	this->buf = (void *) 0xdeadbeef;
	this->len = 0;
}

/* Resizes the string's buffer to be `size' characters long. */
def(void, Resize, size_t size) {
	if (this->buf == NULL) {
		this->buf = Memory_New(size);
	} else {
		this->buf = Memory_Resize(this->buf, size);
	}

	if (this->len > size) {
		/* The string was shortened. */
		this->len = size;
	}
}

/* Ensures that the string can hold `size' characters. */
def(void, Align, size_t size) {
	if (this->len >= size) {
		return;
	}

	if (this->buf == NULL) {
		this->buf = Memory_New(size);
		return;
	}

	size_t actual = Memory_GetSize(this->buf);

	/* Requirement for the do..while loop. */
	assert(actual != 0);

	if (size > actual) {
#if String_SmartAlign
		/* See also:
		 * http://stackoverflow.com/questions/2243366/how-to-implement-a-variable-length-string-y-in-c
		 */
		do {
			actual <<= 1;
		} while (actual < size);

		call(Resize, actual);
#else
		call(Resize, size);
#endif
	}
}

sdef(self, Clone, RdString s) {
	self out = { .len = s.len };

	if (s.len > 0) {
		out.buf = Memory_New(s.len);
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

overload def(void, Crop, ssize_t offset, ssize_t length) {
	size_t right;

	if (offset < 0) {
		offset += this->len;
	}

	if (length < 0) {
		right = length + this->len;
	} else {
		right = length + offset;
	}

	if ((size_t) offset > right     ||
		(size_t) offset > this->len ||
		right           > this->len)
	{
		throw(BufferOverflow);
	}

	assert(right - offset > 0);

	if (offset > 0) {
		assert(this->buf != NULL);

		/* Memory_Move is preferable because it also works with overlapping
		 * memory areas.
		 */

		Memory_Move(this->buf,
			this->buf + offset,
			right     - offset);
	}

	this->len = right - offset;
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

overload def(void, Prepend, RdString s) {
	self tmp = scall(Concat, s, this->rd);
	call(Copy, tmp.rd);
	scall(Destroy, &tmp);
}

overload def(void, Prepend, char c) {
	self tmp = String_New(this->len + 1);
	scall(Append, &tmp, c);
	scall(Append, &tmp, this->rd);
	call(Copy, tmp.rd);
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

overload def(void, Append, RdString s) {
	if (s.len == 0) {
		return;
	}

	/* Resize the buffer if necessary. */
	call(Align, this->len + s.len);

	/* Append s. */
	Memory_Copy(this->buf + this->len, s.buf, s.len);
	this->len += s.len;
}

overload def(void, Append, char c) {
	call(Align, this->len + 1);
	this->buf[this->len] = c;
	this->len++;
}

overload def(void, Append, FmtString s) {
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

	call(Align, len);

	fwd(i, s.fmt.len) {
		if (i + 1 < s.fmt.len && s.fmt.buf[i] == '!' && s.fmt.buf[i + 1] == '%') {
			this->buf[this->len] = '%';
			this->len++, i++;
		} else if (s.fmt.buf[i] == '%') {
			Memory_Copy(this->buf + this->len, val->buf, val->len);
			this->len += val->len;
			val++;
		} else {
			this->buf[this->len] = s.fmt.buf[i];
			this->len++;
		}
	}
}

overload sdef(bool, BeginsWith, RdString s, RdString needle) {
	if (needle.len == 0) {
		return true;
	} else if (needle.len > s.len) {
		return false;
	}

	return Memory_Equals(s.buf, needle.buf, needle.len);
}

overload sdef(bool, BeginsWith, RdString s, char needle) {
	if (s.len == 0) {
		return false;
	}

	return s.buf[0] == needle;
}

overload sdef(bool, EndsWith, RdString s, RdString needle) {
	if (needle.len == 0) {
		return true;
	} else if (needle.len > s.len) {
		return false;
	}

	return Memory_Equals(s.buf + s.len - needle.len, needle.buf, needle.len);
}

overload sdef(bool, EndsWith, RdString s, char needle) {
	if (s.len == 0) {
		return false;
	}

	return s.buf[s.len - 1] == needle;
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

				assert(value != NULL);

				value->buf = subject.buf + ofs;
				value->len = subject.len - ofs;

				ssize_t ofsNext = String_Find(String_Slice(pattern, i), '%');

				if (ofsNext != String_NotFound) {
					ofsNext += i;

					RdString param = String_Slice(pattern, i, ofsNext - i);

					ssize_t ofsParam = String_Find(*value, param);

					if (ofsParam != String_NotFound) {
						value->len = ofsParam;
						subj = ofsParam + ofs;
					}
				} else {
					fwd(j, value->len) {
						if (value->buf[j] == pattern.buf[i]) {
							value->len = j;
							subj = j + ofs;
							break;
						}
					}
				}

				bool found = (value->len != subject.len - ofs);

				if (!found) {
					VarArg_End(argptr);
					return false;
				}

				ofs = -1;
			}

			if (subj >= subject.len ||
				subject.buf[subj] != pattern.buf[i])
			{
				VarArg_End(argptr);
				return false;
			}

			subj++;
		}
	}

	if (ofs != -1) {
		RdString *value = VarArg_Get(argptr, RdString *);

		assert(value != NULL);

		value->buf = subject.buf + ofs;
		value->len = subject.len - ofs;
	}

	VarArg_End(argptr);

	return true;
}

overload sdef(ssize_t, Find, RdString s, char c) {
	fwd(i, s.len) {
		if (s.buf[i] == c) {
			return i;
		}
	}

	return ref(NotFound);
}

overload sdef(ssize_t, ReverseFind, RdString s, char c) {
	bwd(i, s.len) {
		if (s.buf[i] == c) {
			return i;
		}
	}

	return ref(NotFound);
}

overload sdef(ssize_t, Find, RdString s, RdString needle) {
	if (needle.len == 0 || needle.len > s.len) {
		return ref(NotFound);
	} else if (needle.len == s.len) {
		return !Memory_Equals(s.buf, needle.buf, needle.len)
			? ref(NotFound)
			: 0;
	}

	size_t cnt = 0;

	fwd(i, s.len) {
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

overload sdef(ssize_t, ReverseFind, RdString s, RdString needle) {
	if (needle.len == 0 || needle.len > s.len) {
		return ref(NotFound);
	} else if (needle.len == s.len) {
		return !Memory_Equals(s.buf, needle.buf, needle.len)
			? ref(NotFound)
			: 0;
	}

	size_t cnt = 0;

	bwd(i, s.len) {
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

sdef(bool, Between, RdString s, RdString left, RdString right, RdString *res) {
	assert(res != NULL);

	size_t offset = (res->buf != NULL)
		? res->buf - s.buf + res->len + right.len
		: 0;

	res->len = 0;

	if (s.len == 0 || offset > s.len) {
		return false;
	}

	ssize_t posLeft = scall(Find, (RdString) {
		.buf = s.buf + offset,
		.len = s.len - offset
	}, left);

	if (posLeft == ref(NotFound)) {
		return false;
	}

	posLeft += offset + 1;

	ssize_t posRight = scall(Find, (RdString) {
		.buf = s.buf + posLeft,
		.len = s.len - posLeft
	}, right);

	if (posRight == ref(NotFound)) {
		return false;
	}

	posRight += posLeft;

	*res = (RdString) {
		.buf = s.buf    + posLeft,
		.len = posRight - posLeft
	};

	return true;
}

sdef(RdString, Cut, RdString s, RdString left, RdString right) {
	ssize_t posLeft = scall(Find, s, left);

	if (posLeft == ref(NotFound)) {
		return $("");
	}

	ssize_t posRight = scall(Find, String_Slice(s, posLeft + left.len), right);

	if (posRight == ref(NotFound)) {
		return $("");
	}

	posRight += posLeft + left.len;

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

	if ((right = scall(Find, String_Slice(this->rd, left), s2)) == ref(NotFound)) {
		return false;
	}

	right += left;

	scall(Append, &out, String_Slice(this->rd, left, right - left));
	scall(Append, &out, String_Slice(this->rd, right + s2.len));

	call(Copy, out.rd);

	scall(Destroy, &out);

	return true;
}

def(bool, Outside, RdString left, RdString right) {
	ssize_t posLeft, posRight;

	if ((posLeft = scall(Find, this->rd, left)) == ref(NotFound)) {
		return false;
	}

	if ((posRight = scall(Find, String_Slice(this->rd, posLeft + left.len), right)) == ref(NotFound)) {
		return false;
	}

	posRight += posLeft + left.len;

	self out = String_New(posLeft + this->len - posRight - right.len);

	scall(Append, &out, String_Slice(this->rd, 0, posLeft));
	scall(Append, &out, String_Slice(this->rd, posRight + right.len));

	call(Copy, out.rd);

	scall(Destroy, &out);

	return true;
}

overload sdef(self, Concat, RdString a, RdString b) {
	self res = String_New(a.len + b.len);

	Memory_Copy(res.buf, a.buf, a.len);
	res.len = a.len;

	Memory_Copy(res.buf + res.len, b.buf, b.len);
	res.len += b.len;

	return res;
}

overload sdef(self, Concat, RdString s, char c) {
	self res = String_New(s.len + 1);

	Memory_Copy(res.buf, s.buf, s.len);

	res.buf[s.len] = c;

	res.len = s.len + 1;

	return res;
}

sdef(CarrierString, Replace, RdString s, RdString needle, RdString replacement) {
	ssize_t pos = scall(Find, s, needle);

	if (pos == ref(NotFound)) {
		return String_ToCarrier(RdString_Exalt(s));
	}

	self out = String_New(s.len - needle.len + replacement.len);

	Memory_Copy(out.buf, s.buf, pos);
	out.len += pos;

	Memory_Copy(out.buf + out.len, replacement.buf, replacement.len);
	out.len += replacement.len;

	Memory_Copy(out.buf + out.len,
		s.buf + pos + needle.len,
		s.len - pos - needle.len);
	out.len += s.len - pos - needle.len;

	return String_ToCarrier(out);
}

sdef(CarrierString, ReplaceAll, RdString s, RdString needle, RdString replacement) {
	CarrierString out = String_ToCarrier(RdString_Exalt(s));

	if (needle.len > s.len) {
		return out;
	}

	size_t len     = s.len - needle.len + replacement.len;
	size_t cnt     = 0;
	size_t lastPos = 0;

	fwd(i, s.len) {
		if (s.buf[i] == needle.buf[cnt]) {
			cnt++;

			if (cnt == needle.len) {
				if (out.omni) {
					out = String_ToCarrier(String_New(len));
				}

				size_t cur = i - needle.len + 1;

				scall(Append, (String *) &out, String_Slice(s, lastPos, cur - lastPos));
				scall(Append, (String *) &out, replacement);

				lastPos = i + 1;
				cnt = 0;
			}
		} else if (cnt > 0) {
			cnt = 0;
			i--;
		}
	}

	if (lastPos != 0) {
		scall(Append, (String *) &out, String_Slice(s, lastPos));
	}

	return out;
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
			short res;

			RdString __a = scall(Slice, a, ai);
			RdString __b = scall(Slice, b, bi);

			if (!skipZeros) {
				/* Is fractional? */
				if (a.buf[ai] == '0' || b.buf[bi] == '0') {
					res = scall(CompareLeft, __a, __b);

					if (res != 0) {
						return res;
					}
				}
			}

			res = scall(CompareRight, __a, __b);

			if (res != 0) {
				return res;
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

sdef(short, Compare, RdString left, RdString right) {
	short len = Min(left.len, right.len);
	return memcmp(left.buf, right.buf, len);
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
