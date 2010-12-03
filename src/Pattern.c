#import "Pattern.h"

#define self Pattern

/*
 * Copyright (c) 2004-2005 Sergey Lyubka <valenok@gmail.com>
 * All rights reserved
 *
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Sergey Lyubka wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 */

static const String metaChars = String("|.^$*+?()[\\");

def(void, Init) {
	this->code = scall(CharArray_New, 256);
	this->data = scall(CharArray_New, 256);

	this->ofs     = 0;
	this->numCaps = 0;
}

def(void, Destroy) {
	scall(CharArray_Free, this->code);
	scall(CharArray_Free, this->data);
}

// --------
// Compiler
// --------

static def(void, SetJumpOffset, size_t pc, size_t offset) {
	if (offset >= this->code->len) {
		throw(excOffsetOverflow);
	} else if (this->code->len - offset > 0xff) {
		throw(excJumpOffsetTooBig);
	} else {
		this->code->buf[pc] = (unsigned char) (this->code->len - offset);
	}
}

static def(void, Emit, int code) {
	scall(CharArray_Push, &this->code, (unsigned char) code);
}

static def(void, StoreCharInData, int ch) {
	scall(CharArray_Push, &this->data, (unsigned char) ch);
}

static def(size_t, Exact, size_t offset, String pattern) {
	size_t oldDataSize = this->data->len;

	while (String_Find(metaChars, pattern.buf[offset]) == String_NotFound) {
		call(StoreCharInData, pattern.buf[offset]);

		if (offset == pattern.len - 1) {
			break;
		}

		offset++;
	}

	call(Emit, ref(Token_Exact));
	call(Emit, oldDataSize);
	call(Emit, this->data->len - oldDataSize);

	if (offset == pattern.len - 1) {
		return offset;
	} else {
		return offset - 1;
	}
}

static sdef(int, GetEscapeChar, char c) {
	switch (c) {
		case 'n': return '\n';
		case 'r': return '\r';
		case 't': return '\t';
		case '0': return 0;
		case 'S': return ref(Token_NonSpace) << 8;
		case 's': return ref(Token_Space)    << 8;
		case 'd': return ref(Token_Digit)    << 8;
	}

	return c;
}

static def(size_t, AnyOf, size_t offset, String pattern) {
	ref(Token) op = ref(Token_AnyOf);
	size_t oldDataSize = this->data->len;

	if (pattern.buf[offset] == '^') {
		op = ref(Token_AnyBut);
		offset++;
	}

	for (; offset < pattern.len; offset++) {
		if (pattern.buf[offset] == ']') {
			call(Emit, op);
			call(Emit, oldDataSize);
			call(Emit, this->data->len - oldDataSize);

			break;
		} else if (pattern.buf[offset] == '\\') {
			int esc = scall(GetEscapeChar, pattern.buf[offset + 1]);

			if ((esc & 0xff) == 0) {
				call(StoreCharInData, 0);
				call(StoreCharInData, esc >> 8);
			} else {
				call(StoreCharInData, esc);
			}
		} else {
			call(StoreCharInData, pattern.buf[offset]);
		}
	}

	if (offset == pattern.len - 1) {
		return offset;
	} else {
		return offset - 1;
	}
}

static def(void, Relocate, size_t begin, size_t shift) {
	call(Emit, ref(Token_End));

	if (begin + shift           > this->code->size
	 || this->code->len - begin > this->code->len) {
		throw(excOffsetOverflow);
	}

	Memory_Move(
		this->code->buf + begin + shift,
		this->code->buf + begin,
		this->code->len - begin);

	this->code->len += shift;
}

static def(void, Quantifier, size_t prev, ref(Token) op) {
	if (this->code->buf[prev] == ref(Token_Exact) && this->code->buf[prev + 2] > 1) {
		this->code->buf[prev + 2]--;

		call(Emit, ref(Token_Exact));

		call(Emit,
			this->code->buf[prev + 1] +
			this->code->buf[prev + 2]);

		call(Emit, 1);

		prev = this->code->len - 3;
	}

	call(Relocate, prev, 2);
	this->code->buf[prev] = (unsigned char) op;
	call(SetJumpOffset, prev + 1, prev);
}

static def(void, ExactOneChar, int ch) {
	call(Emit, ref(Token_Exact));
	call(Emit, this->data->len);
	call(Emit, 1);

	call(StoreCharInData, ch);
}

static def(void, FixUpBranch, ssize_t fixup) {
	if (fixup > 2) {
		call(Emit, ref(Token_End));
		call(SetJumpOffset, fixup, fixup - 2);
	}
}

static def(size_t, Parse, size_t offset, String pattern) {
	size_t fixup       = 0;
	size_t level       = this->numCaps;
	size_t lastOp      = this->code->len;
	size_t branchStart = this->code->len;

	for (; offset < pattern.len; offset++) {
		if (pattern.buf[offset] == '^') {
			call(Emit, ref(Token_BOL));
		} else if (pattern.buf[offset] == '$') {
			call(Emit, ref(Token_EOL));
		} else if (pattern.buf[offset] == '.') {
			lastOp = this->code->len;
			call(Emit, ref(Token_Any));
		} else if (pattern.buf[offset] == '[') {
			lastOp = this->code->len;
			offset = call(AnyOf, offset + 1, pattern);

			if (pattern.buf[offset] != ']') {
				throw(excNoClosingBracket);
			}
		} else if (pattern.buf[offset] == '\\') {
			offset++;

			if (offset == pattern.len - 1) {
				break;
			}

			lastOp = this->code->len;

			int esc = scall(GetEscapeChar, pattern.buf[offset]);

			if (esc & 0xff00) {
				call(Emit, esc >> 8);
			} else {
				call(ExactOneChar, esc);
			}
		} else if (pattern.buf[offset] == '(') {
			lastOp = this->code->len;

			size_t capNo = ++this->numCaps;

			call(Emit, ref(Token_Open));
			call(Emit, capNo);

			offset = call(Parse, offset + 1, pattern);

			if (pattern.buf[offset] != ')') {
				throw(excNoClosingBracket);
			}

			call(Emit, ref(Token_Close));
			call(Emit, capNo);
		} else if (pattern.buf[offset] == ')') {
			call(FixUpBranch, fixup);

			if (level == 0) {
				throw(excUnbalancedBrackets);
			}

			return offset;
		} else if (pattern.buf[offset] == '+'
				|| pattern.buf[offset] == '*')
		{
			ref(Token) op = (pattern.buf[offset] == '*')
				? ref(Token_Star)
				: ref(Token_Plus);

			if (pattern.buf[offset + 1] == '?') {
				op = (op == ref(Token_Star))
					? ref(Token_StarQ)
					: ref(Token_PlusQ);

				offset++;
			}

			call(Quantifier, lastOp, op);
		} else if (pattern.buf[offset] == '?') {
			call(Quantifier, lastOp, ref(Token_QuestionMark));
		} else if (pattern.buf[offset] == '|') {
			call(FixUpBranch, fixup);
			call(Relocate, branchStart, 3);
			this->code->buf[branchStart] = ref(Token_Branch);
			call(SetJumpOffset, branchStart + 1, branchStart);
			fixup = branchStart + 2;
			this->code->buf[fixup] = 0xff;
		} else {
			lastOp = this->code->len;
			offset = call(Exact, offset, pattern);
		}
	}

	if (offset == pattern.len - 1) {
		return offset;
	} else {
		return offset - 1;
	}
}

def(void, Compile, String pattern) {
	if (pattern.len == 0) {
		throw(excEmptyPattern);
	}

	this->numCaps   = 0;
	this->code->len = 0;
	this->data->len = 0;

	call(Emit, ref(Token_Open));
	call(Emit, 0);

	call(Parse, 0, pattern);

	if (this->code->buf[2] == ref(Token_Branch)) {
		call(FixUpBranch, 4);
	}

	call(Emit, ref(Token_Close));
	call(Emit, 0);

	call(Emit, ref(Token_End));
}

// -------
// Matcher
// -------

static def(bool, _Match, size_t pc, String s, size_t len, String **caps);

static def(void, LoopGreedy, size_t pc, String s, size_t len) {
	size_t savedOffset;
	size_t matchedOffset = this->ofs;

	while (call(_Match, pc + 2, s, len, NULL)) {
		savedOffset = this->ofs;

		if (call(_Match, pc + this->code->buf[pc + 1], s, len, NULL)) {
			matchedOffset = savedOffset;
		}

		this->ofs = savedOffset;
	}

	this->ofs = matchedOffset;
}

static def(void, LoopNonGreedy, size_t pc, String s, size_t len) {
	size_t savedOffset = this->ofs;

	while (call(_Match, pc + 2, s, len, NULL)) {
		savedOffset = this->ofs;

		if (call(_Match, pc + this->code->buf[pc + 1], s, len, NULL)) {
			break;
		}
	}

	this->ofs = savedOffset;
}

static def(bool, IsAnyOf, const unsigned char *p, String s, size_t len) {
	int ch = s.buf[this->ofs];

	for (size_t i = 0; i < len; i++) {
		if (p[i] == ch) {
			this->ofs++;
			return true;
		}
	}

	return false;
}

static def(bool, IsAnyBut, const unsigned char *p, String s, size_t len) {
	int ch = s.buf[this->ofs];

	for (size_t i = 0; i < len; i++) {
		if (p[i] == ch) {
			return false;
		}
	}

	this->ofs++;

	return true;
}

static def(bool, _Match, size_t pc, String s, size_t len, String **caps) {
	size_t savedOffset;

	bool res = true;

	while (res && this->code->buf[pc] != ref(Token_End)) {
		if (pc >= this->code->len) {
			throw(excOffsetOverflow);
		}

		switch (this->code->buf[pc]) {
		case ref(Token_Branch):
			savedOffset = this->ofs;
			res = call(_Match, pc + 3, s, len, caps);

			if (!res) {
				this->ofs = savedOffset;
				res = call(_Match, pc + this->code->buf[pc + 1], s, len, caps);
			}

			pc += this->code->buf[pc + 2];
			break;

		case ref(Token_Exact):
			res = false;

			size_t dataOffset = this->code->buf[pc + 1];
			size_t dataLength = this->code->buf[pc + 2];

			String cmp = BufString(
				(char *) this->data->buf + dataOffset,
				dataLength);

			String orig = BufString(
				s.buf + this->ofs, dataLength);

			if (dataLength <= len - this->ofs && String_Equals(orig, cmp)) {
				this->ofs += dataLength;
				res = true;
			}

			pc += 3;
			break;

		case ref(Token_QuestionMark):
			res = true;

			savedOffset = this->ofs;

			if (!call(_Match, pc + 2, s, len, caps)) {
				this->ofs = savedOffset;
			}

			pc += this->code->buf[pc + 1];
			break;

		case ref(Token_Star):
			res = true;
			call(LoopGreedy, pc, s, len);
			pc += this->code->buf[pc + 1];
			break;

		case ref(Token_StarQ):
			res = true;
			call(LoopNonGreedy, pc, s, len);
			pc += this->code->buf[pc + 1];
			break;

		case ref(Token_Plus):
			if (!(res = call(_Match, pc + 2, s, len, caps))) {
				break;
			}

			call(LoopGreedy, pc, s, len);
			pc += this->code->buf[pc + 1];
			break;

		case ref(Token_PlusQ):
			res = call(_Match, pc + 2, s, len, caps);

			if (!res) {
				break;
			}

			call(LoopNonGreedy, pc, s, len);
			pc += this->code->buf[pc + 1];
			break;

		case ref(Token_Space):
			res = false;

			if (this->ofs < len && Char_IsSpace(s.buf[this->ofs])) {
				this->ofs++;
				res = true;
			}

			pc++;
			break;

		case ref(Token_NonSpace):
			res = false;

			if (this->ofs < len && !Char_IsSpace(s.buf[this->ofs])) {
				this->ofs++;
				res = true;
			}

			pc++;
			break;

		case ref(Token_Digit):
			res = false;

			if (this->ofs < len && Char_IsDigit(s.buf[this->ofs])) {
				this->ofs++;
				res = true;
			}

			pc++;
			break;

		case ref(Token_Any):
			res = false;

			if (this->ofs < len) {
				this->ofs++;
				res = true;
			}

			pc++;
			break;

		case ref(Token_AnyOf):
			res = false;

			if (this->ofs < len) {
				size_t offset  = this->code->buf[pc + 1];
				size_t offset2 = this->code->buf[pc + 2];

				res = call(IsAnyOf,
					this->data->buf + offset,
					s, offset2);
			}

			pc += 3;
			break;

		case ref(Token_AnyBut):
			res = false;

			if (this->ofs < len) {
				size_t offset  = this->code->buf[pc + 1];
				size_t offset2 = this->code->buf[pc + 2];

				res = call(IsAnyBut,
					this->data->buf + offset,
					s, offset2);
			}

			pc += 3;

			break;

		case ref(Token_BOL):
			res = (this->ofs == 0);
			pc++;
			break;

		case ref(Token_EOL):
			res = (this->ofs == len);
			pc++;
			break;

		case ref(Token_Open):
			if (caps != NULL) {
				size_t offset = this->code->buf[pc + 1];

				if (caps[offset] != NULL) {
					caps[offset]->buf     = s.buf + this->ofs;
					caps[offset]->len     = 0;
					caps[offset]->mutable = false;
				}
			}

			pc += 2;
			break;

		case ref(Token_Close):
			if (caps != NULL) {
				size_t offset = this->code->buf[pc + 1];

				if (caps[offset] != NULL) {
					size_t orig = caps[offset]->buf - s.buf;
					caps[offset]->len = this->ofs - orig;
				}
			}

			pc += 2;
			break;

		case ref(Token_End):
			pc++;
			break;

		default:
			throw(excUnknownCommand);
		}
	}

	return res;
}

def(bool, Match, String s, String **caps) {
	this->ofs = 0;

	if (this->code->buf[2] != ref(Token_BOL)) {
		/* Pattern is anchored (must match from the start). */
		return call(_Match, 0, s, s.len, caps);
	} else {
		for (size_t i = 0; i < s.len; i++) {
			this->ofs = i;

			if (call(_Match, 0, s, s.len, caps)) {
				return true;
			}
		}

		return false;
	}
}
