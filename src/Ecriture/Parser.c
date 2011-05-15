#import "Parser.h"

#define self Ecriture_Parser

rsdef(self, New, Ecriture_OnToken onToken) {
	return (self) {
		.onToken = onToken
	};
}

def(void, Destroy) { }

static def(void, ParseAttrValue) {
	char c;
	char prev = '\0';
	RdString value = $("");

	size_t line = StringReader_GetLine(&this->reader);

	while (StringReader_Peek(&this->reader, &c)) {
		if (c == ']' && prev != '`') {
			StringReader_Consume(&this->reader);
			callback(this->onToken, Ecriture_TokenType_AttrValue, value, line);
			break;
		} else {
			StringReader_Extend(&this->reader, &value);
		}

		prev = c;
	}
}

static def(void, ParseOption) {
	char c;
	char prev = '\0';
	RdString value = $("");

	size_t line = StringReader_GetLine(&this->reader);

	while (StringReader_Peek(&this->reader, &c)) {
		if (c == ']' && prev != '`') {
			StringReader_Consume(&this->reader);
			callback(this->onToken, Ecriture_TokenType_Option, value, line);
			break;
		} else if (c == '=' && prev != '`') {
			StringReader_Consume(&this->reader);
			callback(this->onToken, Ecriture_TokenType_AttrName, value, line);
			call(ParseAttrValue);
			break;
		} else {
			StringReader_Extend(&this->reader, &value);
		}

		prev = c;
	}
}

static def(void, ParseComment) {
	char c;
	char next = '\0';
	RdString comment = $("");

	size_t line = StringReader_GetLine(&this->reader);

	while (StringReader_Peek(&this->reader, &c)) {
		StringReader_Peek(&this->reader, &next, 1);

		if (c == '*' && next == '/') {
			StringReader_Consume(&this->reader);
			StringReader_Consume(&this->reader);

			callback(this->onToken, Ecriture_TokenType_Comment, comment, line);
			break;
		} else {
			StringReader_Extend(&this->reader, &comment);
		}
	}
}

static def(void, ParseLiteral) {
	char c;
	char next = '\0';
	RdString value = $("");

	size_t line = StringReader_GetLine(&this->reader);

	while (StringReader_Peek(&this->reader, &c)) {
		StringReader_Peek(&this->reader, &next, 1);

		if (c == '`' && next == '`') {
			StringReader_Extend(&this->reader, &value);
			StringReader_Extend(&this->reader, &value);
		} else if (c == '`') {
			StringReader_Consume(&this->reader);
			callback(this->onToken, Ecriture_TokenType_Literal, value, line);
			break;
		} else {
			StringReader_Extend(&this->reader, &value);
		}
	}
}

static def(void, Parse, bool inTag);

static def(void, ParseTag) {
	char c;
	RdString name = $("");

	size_t line = StringReader_GetLine(&this->reader);

	while (StringReader_Peek(&this->reader, &c)) {
		if (c == '[' || c == '{') {
			StringReader_Consume(&this->reader);

			if (name.len != 0) {
				callback(this->onToken, Ecriture_TokenType_TagStart,
					String_Trim(name), line);
				name.len = 0;
			}
		}

		if (c == '[') {
			call(ParseOption);
		} else if (c == '{') {
			call(Parse, true);
			callback(this->onToken, Ecriture_TokenType_TagEnd, $(""), line);
			break;
		} else {
			StringReader_Extend(&this->reader, &name);
		}
	}
}

static def(void, Parse, bool inTag) {
	char c, next;
	RdString value = $("");

	size_t line = StringReader_GetLine(&this->reader);

	while (StringReader_Peek(&this->reader, &c)) {
		StringReader_Peek(&this->reader, &next, 1);

		if (c == '`') {
			StringReader_Consume(&this->reader);

			if (value.len != 0) {
				callback(this->onToken, Ecriture_TokenType_Value, value, line);
				value.len = 0;
			}

			call(ParseLiteral);
		} else if (c == '.' && (Char_IsAlpha(next) || Char_IsDigit(next))) {
			StringReader_Consume(&this->reader);

			if (value.len != 0) {
				callback(this->onToken, Ecriture_TokenType_Value, value, line);
				value.len = 0;
			}

			call(ParseTag);
		} else if (c == '/' && next == '*') {
			StringReader_Consume(&this->reader);
			StringReader_Consume(&this->reader);

			if (value.len != 0) {
				callback(this->onToken, Ecriture_TokenType_Value, value, line);
				value.len = 0;
			}

			call(ParseComment);
		} else if (inTag && c == '}') {
			StringReader_Consume(&this->reader);
			break;
		} else {
			StringReader_Extend(&this->reader, &value);
		}
	}

	if (value.len != 0) {
		callback(this->onToken, Ecriture_TokenType_Value, value, line);
	}
}

def(void, Process, RdString s) {
	this->reader = StringReader_New(s);

	call(Parse, false);

	size_t line = StringReader_GetLine(&this->reader);
	callback(this->onToken, Ecriture_TokenType_Done, $(""), line);
}
