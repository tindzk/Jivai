#import "Parser.h"

#define self Ecriture_Parser

rsdef(self, New, Ecriture_OnToken onToken) {
	return (self) {
		.onToken = onToken
	};
}

def(void, Destroy) { }

overload def(bool, Peek, char *c) {
	assert(c != NULL);

	if (this->ofs >= this->buf.len) {
		*c = '\0';
		return false;
	}

	*c = this->buf.buf[this->ofs];

	return true;
}

overload def(bool, Peek, char *c, size_t cnt) {
	assert(c != NULL);

	if (this->ofs + cnt >= this->buf.len) {
		*c = '\0';
		return false;
	}

	*c = this->buf.buf[this->ofs + cnt];

	return true;
}

overload def(void, Consume) {
	assert(this->ofs + 1 <= this->buf.len);

	if (this->buf.buf[this->ofs] == '\n') {
		this->line++;
	}

	this->ofs++;
}

def(void, Extend, RdString *str) {
	assert(str != NULL);

	if (str->len == 0) {
		*str = String_Slice(this->buf, this->ofs, 1);
	} else {
		size_t ofs = str->buf - this->buf.buf;
		assert(ofs + str->len + 1 <= this->buf.len);
		str->len++;
	}

	if (this->buf.buf[this->ofs] == '\n') {
		this->line++;
	}

	this->ofs++;
}

static def(void, ParseOption) {
	char c;
	char prev = '\0';
	RdString value = $("");

	while (call(Peek, &c)) {
		if (c == ']' && prev != '`') {
			call(Consume);
			callback(this->onToken, Ecriture_TokenType_Option, value, this->line);
			break;
		} else {
			call(Extend, &value);
		}

		prev = c;
	}
}

static def(void, ParseLiteral) {
	char c;
	char next = '\0';
	RdString value = $("");

	while (call(Peek, &c)) {
		call(Peek, &next, 1);

		if (c == '`' && next == '`') {
			call(Extend, &value);
			call(Extend, &value);
		} else if (c == '`') {
			call(Consume);
			callback(this->onToken, Ecriture_TokenType_Literal, value, this->line);
			break;
		} else {
			call(Extend, &value);
		}
	}
}

static def(void, Parse, bool inTag);

static def(void, ParseTag) {
	char c;
	RdString name = $("");

	while (call(Peek, &c)) {
		if (c == '[' || c == '{') {
			call(Consume);

			if (name.len != 0) {
				callback(this->onToken, Ecriture_TokenType_TagStart, name, this->line);
				name.len = 0;
			}
		}

		if (c == '[') {
			call(ParseOption);
		} else if (c == '{') {
			call(Parse, true);
			callback(this->onToken, Ecriture_TokenType_TagEnd, $(""), this->line);
			break;
		} else {
			call(Extend, &name);
		}
	}
}

def(void, Parse, bool inTag) {
	char c, next;
	RdString value = $("");

	while (call(Peek, &c)) {
		if (c == '`') {
			call(Consume);

			if (value.len != 0) {
				callback(this->onToken, Ecriture_TokenType_Value, value, this->line);
				value.len = 0;
			}

			call(ParseLiteral);
		} else if (c == '.' && call(Peek, &next, 1) && (Char_IsAlpha(next) || Char_IsDigit(next))) {
			call(Consume);

			if (value.len != 0) {
				callback(this->onToken, Ecriture_TokenType_Value, value, this->line);
				value.len = 0;
			}

			call(ParseTag);
		} else if (inTag && c == '}') {
			call(Consume);
			break;
		} else {
			call(Extend, &value);
		}
	}

	if (value.len != 0) {
		callback(this->onToken, Ecriture_TokenType_Value, value, this->line);
	}
}

def(void, Process, RdString s) {
	this->ofs  = 0;
	this->buf  = s;
	this->line = 1;

	call(Parse, false);

	callback(this->onToken, Ecriture_TokenType_Done, $(""), this->line);
}
