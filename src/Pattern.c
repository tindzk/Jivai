#import "Pattern.h"

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

size_t Modules_Pattern;

static ExceptionManager *exc;

void Pattern0(ExceptionManager *e) {
	Modules_Pattern = Module_Register(String("Pattern"));

	exc = e;
}

void Pattern_Init(Pattern *this) {
	Array_Init(this->code, 256);
	Array_Init(this->data, 256);

	this->ofs     = 0;
	this->numCaps = 0;
}

void Pattern_Destroy(Pattern *this) {
	Array_Destroy(this->code);
	Array_Destroy(this->data);
}

// --------
// Compiler
// --------

static void Pattern_SetJumpOffset(Pattern *this, size_t pc, size_t offset) {
	if (offset >= this->code->len) {
		throw(exc, excOffsetOverflow);
	} else if (this->code->len - offset > 0xff) {
		throw(exc, excJumpOffsetTooBig);
	} else {
		this->code->buf[pc] = (unsigned char) (this->code->len - offset);
	}
}

static void Pattern_Emit(Pattern *this, int code) {
	Array_Push(this->code, (unsigned char) code);
}

static void Pattern_StoreCharInData(Pattern *this, int ch) {
	Array_Push(this->data, (unsigned char) ch);
}

static size_t Pattern_Exact(Pattern *this, size_t offset, String pattern) {
	size_t oldDataSize = this->data->len;

	while (String_Find(metaChars, pattern.buf[offset]) == String_NotFound) {
		Pattern_StoreCharInData(this, pattern.buf[offset]);

		if (offset == pattern.len - 1) {
			break;
		}

		offset++;
	}

	Pattern_Emit(this, Token_Exact);
	Pattern_Emit(this, oldDataSize);
	Pattern_Emit(this, this->data->len - oldDataSize);

	if (offset == pattern.len - 1) {
		return offset;
	} else {
		return offset - 1;
	}
}

static int Pattern_GetEscapeChar(char c) {
	switch (c) {
		case 'n': return '\n';
		case 'r': return '\r';
		case 't': return '\t';
		case '0': return 0;
		case 'S': return Token_NonSpace << 8;
		case 's': return Token_Space    << 8;
		case 'd': return Token_Digit    << 8;
	}

	return c;
}

static size_t Pattern_AnyOf(Pattern *this, size_t offset, String pattern) {
	Token op = Token_AnyOf;
	size_t oldDataSize = this->data->len;

	if (pattern.buf[offset] == '^') {
		op = Token_AnyBut;
		offset++;
	}

	for (; offset < pattern.len; offset++) {
		if (pattern.buf[offset] == ']') {
			Pattern_Emit(this, op);
			Pattern_Emit(this, oldDataSize);
			Pattern_Emit(this, this->data->len - oldDataSize);

			break;
		} else if (pattern.buf[offset] == '\\') {
			int esc = Pattern_GetEscapeChar(pattern.buf[offset + 1]);

			if ((esc & 0xff) == 0) {
				Pattern_StoreCharInData(this, 0);
				Pattern_StoreCharInData(this, esc >> 8);
			} else {
				Pattern_StoreCharInData(this, esc);
			}
		} else {
			Pattern_StoreCharInData(this, pattern.buf[offset]);
		}
	}

	if (offset == pattern.len - 1) {
		return offset;
	} else {
		return offset - 1;
	}
}

static void Pattern_Relocate(Pattern *this, size_t begin, size_t shift) {
	Pattern_Emit(this, Token_End);

	if (begin + shift           > this->code->size
	 || this->code->len - begin > this->code->len) {
		throw(exc, excOffsetOverflow);
	}

	Memory_Move(
		this->code->buf + begin + shift,
		this->code->buf + begin,
		this->code->len - begin);

	this->code->len += shift;
}

static void Pattern_Quantifier(Pattern *this, size_t prev, Token op) {
	if (this->code->buf[prev] == Token_Exact && this->code->buf[prev + 2] > 1) {
		this->code->buf[prev + 2]--;

		Pattern_Emit(this, Token_Exact);

		Pattern_Emit(this,
			this->code->buf[prev + 1] +
			this->code->buf[prev + 2]);

		Pattern_Emit(this, 1);

		prev = this->code->len - 3;
	}

	Pattern_Relocate(this, prev, 2);
	this->code->buf[prev] = (unsigned char) op;
	Pattern_SetJumpOffset(this, prev + 1, prev);
}

static void Pattern_ExactOneChar(Pattern *this, int ch) {
	Pattern_Emit(this, Token_Exact);
	Pattern_Emit(this, this->data->len);
	Pattern_Emit(this, 1);

	Pattern_StoreCharInData(this, ch);
}

static void Pattern_FixUpBranch(Pattern *this, ssize_t fixup) {
	if (fixup > 2) {
		Pattern_Emit(this, Token_End);
		Pattern_SetJumpOffset(this, fixup, fixup - 2);
	}
}

static size_t Pattern_Parse(Pattern *this, size_t offset, String pattern) {
	size_t fixup       = 0;
	size_t level       = this->numCaps;
	size_t lastOp      = this->code->len;
	size_t branchStart = this->code->len;

	for (; offset < pattern.len; offset++) {
		if (pattern.buf[offset] == '^') {
			Pattern_Emit(this, Token_BOL);
		} else if (pattern.buf[offset] == '$') {
			Pattern_Emit(this, Token_EOL);
		} else if (pattern.buf[offset] == '.') {
			lastOp = this->code->len;
			Pattern_Emit(this, Token_Any);
		} else if (pattern.buf[offset] == '[') {
			lastOp = this->code->len;
			offset = Pattern_AnyOf(this, offset + 1, pattern);

			if (pattern.buf[offset] != ']') {
				throw(exc, excNoClosingBracket);
			}
		} else if (pattern.buf[offset] == '\\') {
			offset++;

			if (offset == pattern.len - 1) {
				break;
			}

			lastOp = this->code->len;

			int esc = Pattern_GetEscapeChar(pattern.buf[offset]);

			if (esc & 0xff00) {
				Pattern_Emit(this, esc >> 8);
			} else {
				Pattern_ExactOneChar(this, esc);
			}
		} else if (pattern.buf[offset] == '(') {
			lastOp = this->code->len;

			size_t capNo = ++this->numCaps;

			Pattern_Emit(this, Token_Open);
			Pattern_Emit(this, capNo);

			offset = Pattern_Parse(this, offset + 1, pattern);

			if (pattern.buf[offset] != ')') {
				throw(exc, excNoClosingBracket);
			}

			Pattern_Emit(this, Token_Close);
			Pattern_Emit(this, capNo);
		} else if (pattern.buf[offset] == ')') {
			Pattern_FixUpBranch(this, fixup);

			if (level == 0) {
				throw(exc, excUnbalancedBrackets);
			}

			return offset;
		} else if (pattern.buf[offset] == '+'
				|| pattern.buf[offset] == '*')
		{
			Token op = (pattern.buf[offset] == '*')
				? Token_Star
				: Token_Plus;

			if (pattern.buf[offset + 1] == '?') {
				op = (op == Token_Star)
					? Token_StarQ
					: Token_PlusQ;

				offset++;
			}

			Pattern_Quantifier(this, lastOp, op);
		} else if (pattern.buf[offset] == '?') {
			Pattern_Quantifier(this, lastOp, Token_QuestionMark);
		} else if (pattern.buf[offset] == '|') {
			Pattern_FixUpBranch(this, fixup);
			Pattern_Relocate(this, branchStart, 3);
			this->code->buf[branchStart] = Token_Branch;
			Pattern_SetJumpOffset(this, branchStart + 1, branchStart);
			fixup = branchStart + 2;
			this->code->buf[fixup] = 0xff;
		} else {
			lastOp = this->code->len;
			offset = Pattern_Exact(this, offset, pattern);
		}
	}

	if (offset == pattern.len - 1) {
		return offset;
	} else {
		return offset - 1;
	}
}

void Pattern_Compile(Pattern *this, String pattern) {
	if (pattern.len == 0) {
		throw(exc, excEmptyPattern);
	}

	this->numCaps   = 0;
	this->code->len = 0;
	this->data->len = 0;

	Pattern_Emit(this, Token_Open);
	Pattern_Emit(this, 0);

	Pattern_Parse(this, 0, pattern);

	if (this->code->buf[2] == Token_Branch) {
		Pattern_FixUpBranch(this, 4);
	}

	Pattern_Emit(this, Token_Close);
	Pattern_Emit(this, 0);

	Pattern_Emit(this, Token_End);
}

// -------
// Matcher
// -------

static bool _Pattern_Match(Pattern *this, size_t pc, String s, size_t len, String **caps);

static void Pattern_LoopGreedy(Pattern *this, size_t pc, String s, size_t len) {
	size_t savedOffset;
	size_t matchedOffset = this->ofs;

	while (_Pattern_Match(this, pc + 2, s, len, NULL)) {
		savedOffset = this->ofs;

		if (_Pattern_Match(this, pc + this->code->buf[pc + 1], s, len, NULL)) {
			matchedOffset = savedOffset;
		}

		this->ofs = savedOffset;
	}

	this->ofs = matchedOffset;
}

static void Pattern_LoopNonGreedy(Pattern *this, size_t pc, String s, size_t len) {
	size_t savedOffset = this->ofs;

	while (_Pattern_Match(this, pc + 2, s, len, NULL)) {
		savedOffset = this->ofs;

		if (_Pattern_Match(this, pc + this->code->buf[pc + 1], s, len, NULL)) {
			break;
		}
	}

	this->ofs = savedOffset;
}

static bool Pattern_IsAnyOf(Pattern *this, const unsigned char *p, String s, size_t len) {
	int ch = s.buf[this->ofs];

	for (size_t i = 0; i < len; i++) {
		if (p[i] == ch) {
			this->ofs++;
			return true;
		}
	}

	return false;
}

static bool Pattern_IsAnyBut(Pattern *this, const unsigned char *p, String s, size_t len) {
	int ch = s.buf[this->ofs];

	for (size_t i = 0; i < len; i++) {
		if (p[i] == ch) {
			return false;
		}
	}

	this->ofs++;

	return true;
}

static bool _Pattern_Match(Pattern *this, size_t pc, String s, size_t len, String **caps) {
	size_t savedOffset;

	bool res = true;

	while (res && this->code->buf[pc] != Token_End) {
		if (pc >= this->code->len) {
			throw(exc, excOffsetOverflow);
		}

		switch (this->code->buf[pc]) {
		case Token_Branch:
			savedOffset = this->ofs;
			res = _Pattern_Match(this, pc + 3, s, len, caps);

			if (!res) {
				this->ofs = savedOffset;
				res = _Pattern_Match(this, pc + this->code->buf[pc + 1], s, len, caps);
			}

			pc += this->code->buf[pc + 2];
			break;

		case Token_Exact:
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

		case Token_QuestionMark:
			res = true;

			savedOffset = this->ofs;

			if (!_Pattern_Match(this, pc + 2, s, len, caps)) {
				this->ofs = savedOffset;
			}

			pc += this->code->buf[pc + 1];
			break;

		case Token_Star:
			res = true;
			Pattern_LoopGreedy(this, pc, s, len);
			pc += this->code->buf[pc + 1];
			break;

		case Token_StarQ:
			res = true;
			Pattern_LoopNonGreedy(this, pc, s, len);
			pc += this->code->buf[pc + 1];
			break;

		case Token_Plus:
			if (!(res = _Pattern_Match(this, pc + 2, s, len, caps))) {
				break;
			}

			Pattern_LoopGreedy(this, pc, s, len);
			pc += this->code->buf[pc + 1];
			break;

		case Token_PlusQ:
			res = _Pattern_Match(this, pc + 2, s, len, caps);

			if (!res) {
				break;
			}

			Pattern_LoopNonGreedy(this, pc, s, len);
			pc += this->code->buf[pc + 1];
			break;

		case Token_Space:
			res = false;

			if (this->ofs < len && Char_IsSpace(s.buf[this->ofs])) {
				this->ofs++;
				res = true;
			}

			pc++;
			break;

		case Token_NonSpace:
			res = false;

			if (this->ofs < len && !Char_IsSpace(s.buf[this->ofs])) {
				this->ofs++;
				res = true;
			}

			pc++;
			break;

		case Token_Digit:
			res = false;

			if (this->ofs < len && Char_IsDigit(s.buf[this->ofs])) {
				this->ofs++;
				res = true;
			}

			pc++;
			break;

		case Token_Any:
			res = false;

			if (this->ofs < len) {
				this->ofs++;
				res = true;
			}

			pc++;
			break;

		case Token_AnyOf:
			res = false;

			if (this->ofs < len) {
				size_t offset  = this->code->buf[pc + 1];
				size_t offset2 = this->code->buf[pc + 2];

				res = Pattern_IsAnyOf(this,
					this->data->buf + offset,
					s, offset2);
			}

			pc += 3;
			break;

		case Token_AnyBut:
			res = false;

			if (this->ofs < len) {
				size_t offset  = this->code->buf[pc + 1];
				size_t offset2 = this->code->buf[pc + 2];

				res = Pattern_IsAnyBut(this,
					this->data->buf + offset,
					s, offset2);
			}

			pc += 3;

			break;

		case Token_BOL:
			res = (this->ofs == 0);
			pc++;
			break;

		case Token_EOL:
			res = (this->ofs == len);
			pc++;
			break;

		case Token_Open:
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

		case Token_Close:
			if (caps != NULL) {
				size_t offset = this->code->buf[pc + 1];

				if (caps[offset] != NULL) {
					size_t orig = caps[offset]->buf - s.buf;
					caps[offset]->len = this->ofs - orig;
				}
			}

			pc += 2;
			break;

		case Token_End:
			pc++;
			break;

		default:
			throw(exc, excUnknownCommand);
		}
	}

	return res;
}

bool Pattern_Match(Pattern *this, String s, String **caps) {
	this->ofs = 0;

	if (this->code->buf[2] != Token_BOL) {
		/* Pattern is anchored (must match from the start). */
		return _Pattern_Match(this, 0, s, s.len, caps);
	} else {
		for (size_t i = 0; i < s.len; i++) {
			this->ofs = i;

			if (_Pattern_Match(this, 0, s, s.len, caps)) {
				return true;
			}
		}

		return false;
	}
}
