#import "Tokenizer.h"

#define self HTML_Tokenizer

rsdef(self, New, ref(OnToken) onToken) {
	return (self) {
		.onToken = onToken
	};
}

def(void, Destroy) { }

overload def(bool, Peek, char *c) {
	assert(c != NULL);

	if (this->ofs >= this->buf.len) {
		return false;
	}

	*c = this->buf.buf[this->ofs];
	return true;
}

overload def(bool, Peek, RdString *str, size_t len) {
	assert(str != NULL);

	if (this->ofs + len - 1 >= this->buf.len) {
		return false;
	}

	*str = String_Slice(this->buf, this->ofs, len);
	return true;
}

overload def(void, Consume) {
	assert(this->ofs + 1 <= this->buf.len);
	this->ofs++;
}

overload def(void, Consume, size_t len) {
	assert(this->ofs + len <= this->buf.len);
	this->ofs += len;
}

def(void, Extend, RdString *str) {
	assert(str != NULL);

	if (str->buf == NULL) {
		*str = String_Slice(this->buf, this->ofs, 1);
	} else {
		size_t ofs = str->buf - this->buf.buf;
		assert(ofs + str->len + 1 <= this->buf.len);
		str->len++;
	}

	this->ofs++;
}

/* Processes one attribute value, then returns. It supports the following types
 * of values:
 *
 * - Values surrounded by quotes (escaping is supported, too):
 *   <a href='path/' />
 *   <a href="path/" />
 *
 * - Values without quotes:
 *   <a href=path/ />
 *   <a href=path />
 *
 * - Special cases like:
 *   <a href=path/>
 *
 * As for the last type, note that slash belongs to the value and hence no end
 * tag will be created. If this notation is chosen, the end tag must be created
 * manually as in:
 *   <a href=http://localhost/>Caption</a>
 */
def(void, ParseAttrValue) {
	char c;
	char prev      = '\0';
	bool quote     = false;
	char quoteType = '\0';
	RdString value = $("");

	/* Skip all leading spaces. */
	while (call(Peek, &c)) {
		if (!Char_IsSpace(c)) {
			break;
		}

		call(Consume);
	}

	while (call(Peek, &c)) {
		if (quote) {
			call(Extend, &value);

			/* As for quoted values, stop as soon as the value is complete. */
			if (c == quoteType && prev != '\\') {
				callback(this->onToken, ref(TokenType_AttrValue), value);
				break;
			}
		} else if (c == '"' || c == '\'') {
			/* Handle quoted values ("value", 'value'). */
			quote     = true;
			quoteType = c;

			call(Extend, &value);
		} else if (Char_IsSpace(c) || c == '>') {
			/* As for unquoted values, we don't have a clear end delimiter
			 * (like ' or "). We'll stop processing after a space or ">".
			 */

			callback(this->onToken, ref(TokenType_AttrValue), value);
			break;
		} else {
			/* Unquoted value. */
			call(Extend, &value);
		}

		prev = c;
	}
}

/* Decides whether the token is a named attribute (with value) or an option. */
def(void, ParseAttr) {
	char c;
	RdString str;
	RdString name = $("");

	while (call(Peek, &c)) {
		if (c == '=') {
			callback(this->onToken, ref(TokenType_AttrName), name);
			call(Consume);

			call(ParseAttrValue);
			break;
		} else if (Char_IsSpace(c) || c == '>' ||
				(call(Peek, &str, 2) && String_Equals(str, $("/>"))))
		{
			if (name.len != 0) {
				callback(this->onToken, ref(TokenType_Option), name);
			}
			break;
		} else {
			call(Extend, &name);
		}
	}
}

/* Parses all attributes until the tag has reached its end. */
def(void, ParseAttrs) {
	char c;

	call(ParseAttr);

	while (call(Peek, &c)) {
		if (Char_IsSpace(c)) {
			call(Consume);
			call(ParseAttr);
		} else {
			break;
		}
	}
}

/* Matches "tagName>", "tagName/>", "tagName attrs>" and "tagName attrs/>". */
def(void, ParseTagStart) {
	char c;
	bool commitName = true;
	RdString str;
	RdString name = $("");

	while (call(Peek, &c)) {
		if (c == '>') {
			call(Consume);

			if (commitName) {
				callback(this->onToken, ref(TokenType_TagStart), name);
			}
			break;
		} else if (call(Peek, &str, 2) && String_Equals(str, $("/>"))) {
			/* This is an XHTML tag like `<br />'. */
			call(Consume, 2);

			if (commitName) {
				callback(this->onToken, ref(TokenType_TagStart), name);
			}

			callback(this->onToken, ref(TokenType_TagEnd), name);
			break;
		} else if (Char_IsSpace(c)) {
			call(Consume);

			if (commitName) {
				callback(this->onToken, ref(TokenType_TagStart), name);
				commitName = false;
			}

			call(ParseAttrs);
		} else {
			call(Extend, &name);
		}
	}
}

/* Matches "tagName>". */
def(void, ParseTagEnd) {
	char c;
	RdString name = $("");

	while (call(Peek, &c)) {
		if (c == '>') {
			call(Consume);
			callback(this->onToken, ref(TokenType_TagEnd), name);
			break;
		} else {
			call(Extend, &name);
		}
	}
}

/* Matches "...-->". */
def(void, ParseComment) {
	char c;
	RdString str;
	RdString comment = $("");

	while (call(Peek, &c)) {
		if (call(Peek, &str, 3) && String_Equals(str, $("-->"))) {
			call(Consume, 3);
			callback(this->onToken, ref(TokenType_Comment), comment);
			break;
		} else {
			call(Extend, &comment);
		}
	}
}

/* Matches "...]]>". */
def(void, ParseData) {
	char c;
	RdString str;
	RdString data = $("");

	while (call(Peek, &c)) {
		if (call(Peek, &str, 3) && String_Equals(str, $("]]>"))) {
			call(Consume, 3);
			callback(this->onToken, ref(TokenType_Data), data);
			break;
		} else {
			call(Extend, &data);
		}
	}
}

/* Matches "...", "/..." and "!--...". */
def(void, ParseTag) {
	char c;
	RdString str;
	call(Peek, &c);

	if (c == '/') {
		call(Consume);
		call(ParseTagEnd);
	} else if (call(Peek, &str, 8) && String_Equals(str, $("![CDATA["))) {
		call(Consume, 8);
		call(ParseData);
	} else if (call(Peek, &str, 3) && String_Equals(str, $("!--"))) {
		call(Consume, 3);
		call(ParseComment);
	} else {
		call(ParseTagStart);
	}
}

/* Matches "..." and "<...". */
def(void, Parse) {
	char c;
	RdString value = $("");

	while (call(Peek, &c)) {
		if (c != '<') {
			call(Extend, &value);
		} else {
			call(Consume);

			/* Flush the value buffer first. */
			if (value.len != 0) {
				callback(this->onToken, ref(TokenType_Value), value);
				value.len = 0;
			}

			call(ParseTag);
		}
	}

	if (value.len != 0) {
		callback(this->onToken, ref(TokenType_Value), value);
	}
}

def(void, Process, RdString s) {
	this->ofs = 0;
	this->buf = s;

	call(Parse);
}
