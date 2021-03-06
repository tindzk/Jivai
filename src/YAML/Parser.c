#import "Parser.h"

#define self YAML_Parser

rsdef(self, New, YAML_OnToken onToken) {
	return (self) {
		.onToken = onToken
	};
}

def(void, Destroy) { }

static inline def(bool, Indent) {
	return (this->depth > this->prevDepth);
}

static def(void, CommitDepth) {
	if (this->depth > this->prevDepth) {
		if (this->depth != this->prevDepth + 1) {
			/* Indented more than one level. */
			throw(InvalidIndention);
		}

		callback(this->onToken, YAML_TokenType_Enter, $(""));
	} else {
		/* If not indented, this won't generate any `leave' tokens. */
		rpt(this->prevDepth - this->depth) {
			callback(this->onToken, YAML_TokenType_Leave, $(""));
		}
	}

	this->prevDepth = this->depth;
	this->depth = 0;
}

static def(void, ParseComment) {
	char c;
	RdString comment = $("");

	while (StringReader_Peek(&this->reader, &c)) {
		if (c == '\n') {
			StringReader_Consume(&this->reader);
			break;
		} else {
			StringReader_Extend(&this->reader, &comment);
		}
	}

	if (comment.len != 0) {
		callback(this->onToken, YAML_TokenType_Comment, comment);
	}
}

static def(void, ParseValue) {
	char c;
	RdString value = $("");

	while (StringReader_Peek(&this->reader, &c)) {
		if (c == '\n') {
			StringReader_Consume(&this->reader);
			break;
		} else if (c == '#') {
			StringReader_Consume(&this->reader);
			call(ParseComment);
		} else {
			StringReader_Extend(&this->reader, &value);
		}
	}

	value = String_Trim(value);

	if (value.len != 0) {
		callback(this->onToken, YAML_TokenType_Value, value);
	}
}

static def(void, Parse) {
	char c;
	RdString name = $("");
	bool canIndent = false;

	while (StringReader_Peek(&this->reader, &c)) {
		if (c == '#') {
			name.len    = 0;
			this->depth = 0;
			canIndent   = false;

			StringReader_Consume(&this->reader);
			call(ParseComment);
		} else if (c == ':') {
			StringReader_Consume(&this->reader);

			if (!canIndent && call(Indent)) {
				throw(InvalidIndention);
			}

			call(CommitDepth);
			callback(this->onToken, YAML_TokenType_Name,
				String_Trim(name, String_TrimRight));

			call(ParseValue);

			name.len    = 0;
			this->depth = 0;
			canIndent   = true;
		} else if (c == ' ' && name.len == 0) {
			/* Spaces cannot be used for indention. */
			throw(SpaceIndention);
		} else if (c == '\t' && name.len == 0) {
			StringReader_Consume(&this->reader);
			this->depth++;
		} else if (c == '\n' && name.len != 0) {
			throw(ColonMissing);
		} else if (name.len != 0 || !Char_IsSpace(c)) {
			StringReader_Extend(&this->reader, &name);
		} else {
			StringReader_Consume(&this->reader);
		}
	}

	if (name.len != 0) {
		throw(ColonMissing);
	}
}

def(void, Process, RdString s) {
	this->depth     = 0;
	this->prevDepth = 0;
	this->reader    = StringReader_New(s);

	call(Parse);

	rpt(this->prevDepth) {
		callback(this->onToken, YAML_TokenType_Leave, $(""));
	}
}
