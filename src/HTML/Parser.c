#import "Parser.h"

#define self HTML_Parser

rsdef(self, New, HTML_OnToken onToken) {
	return (self) {
		.onToken = onToken
	};
}

def(void, Destroy) { }

overload def(bool, Peek, char *c) {
	if (this->ofs >= this->buf.len) {
		return false;
	}

	if (c != NULL) {
		*c = this->buf.buf[this->ofs];
	}

	return true;
}

overload def(bool, Peek, RdString *str, size_t len) {
	if (this->ofs + len - 1 >= this->buf.len) {
		return false;
	}

	if (str != NULL) {
		*str = String_Slice(this->buf, this->ofs, len);
	}

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

	if (str->len == 0) {
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
				callback(this->onToken, HTML_TokenType_AttrValue, value);
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

			callback(this->onToken, HTML_TokenType_AttrValue, value);
			break;
		} else {
			/* Unquoted value. */
			call(Extend, &value);
		}

		prev = c;
	}
}

/* Decides whether the token is a named attribute (with value) or an option. */
def(bool, ParseAttr) {
	char c;
	RdString str;
	RdString name = $("");

	/* Skip all leading spaces. */
	while (call(Peek, &c)) {
		if (!Char_IsSpace(c)) {
			break;
		}

		call(Consume);
	}

	bool space = false;

	while (call(Peek, &c)) {
		if (Char_IsSpace(c)) {
			space = true;
		}

		if (c == '=') {
			callback(this->onToken, HTML_TokenType_AttrName, name);
			call(Consume);

			call(ParseAttrValue);
			return true;
		} else if (c == '>'
				|| (call(Peek, &str, 2) && String_Equals(str, $("/>"))))
		{
			if (name.len != 0) {
				callback(this->onToken, HTML_TokenType_Option, name);
			}

			return false;
		} else if (space) {
			if (!Char_IsSpace(c)) {
				if (name.len != 0) {
					callback(this->onToken, HTML_TokenType_Option, name);
				}

				return true;
			}

			call(Consume);
		} else {
			call(Extend, &name);
		}
	}

	return false;
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
				callback(this->onToken, HTML_TokenType_TagStart, name);
				callback(this->onToken, HTML_TokenType_AttrEnd, name);
			}
			break;
		} else if (call(Peek, &str, 2) && String_Equals(str, $("/>"))) {
			/* This is an XHTML tag like `<br />'. */
			call(Consume, 2);

			if (commitName) {
				callback(this->onToken, HTML_TokenType_TagStart, name);
				callback(this->onToken, HTML_TokenType_AttrEnd, name);
			}

			callback(this->onToken, HTML_TokenType_TagEnd, $(""));
			break;
		} else if (Char_IsSpace(c)) {
			call(Consume);

			if (commitName) {
				callback(this->onToken, HTML_TokenType_TagStart, name);
				commitName = false;
			}

			/* Parse all attributes until the tag has reached its end. */
			while(call(ParseAttr));

			callback(this->onToken, HTML_TokenType_AttrEnd, name);
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
			callback(this->onToken, HTML_TokenType_TagEnd, name);
			break;
		} else {
			call(Extend, &name);
		}
	}
}

/* Matches "...-->". */
def(void, ParseComment) {
	RdString str;
	RdString comment = $("");

	while (call(Peek, NULL)) {
		if (call(Peek, &str, 3) && String_Equals(str, $("-->"))) {
			call(Consume, 3);
			callback(this->onToken, HTML_TokenType_Comment, comment);
			break;
		} else {
			call(Extend, &comment);
		}
	}
}

/* Matches "...]]>". */
def(void, ParseData) {
	RdString str;
	RdString data = $("");

	while (call(Peek, NULL)) {
		if (call(Peek, &str, 3) && String_Equals(str, $("]]>"))) {
			call(Consume, 3);
			callback(this->onToken, HTML_TokenType_Data, data);
			break;
		} else {
			call(Extend, &data);
		}
	}
}

/* Matches "...>". */
def(void, ParseType) {
	char c;
	RdString type = $("");

	while (call(Peek, &c)) {
		if (c == '>') {
			call(Consume);
			callback(this->onToken, HTML_TokenType_Type, type);
			break;
		} else {
			call(Extend, &type);
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
	} else if (call(Peek, &str, 9) && String_Equals(str, $("!DOCTYPE "))) {
		call(Consume, 9);
		call(ParseType);
	} else if (call(Peek, &str, 3) && String_Equals(str, $("!--"))) {
		call(Consume, 3);
		call(ParseComment);
	} else {
		call(ParseTagStart);
	}
}

/* Allows loose and invalid `<'s. */
def(bool, IsTag) {
	RdString str;

	if (call(Peek, &str, 2)) {
		if (str.buf[0] == '<') {
			if (str.buf[1] == '!' || str.buf[1] == '/' || Char_IsAlpha(str.buf[1])) {
				return true;
			}
		}
	}

	return false;
}

/* Matches "..." and "<...". */
def(void, Parse) {
	char c;
	RdString value = $("");

	while (call(Peek, &c)) {
		if (!call(IsTag)) {
			call(Extend, &value);
		} else {
			call(Consume);

			/* Flush the value buffer first. */
			if (value.len != 0) {
				callback(this->onToken, HTML_TokenType_Value, value);
				value.len = 0;
			}

			call(ParseTag);
		}
	}

	if (value.len != 0) {
		callback(this->onToken, HTML_TokenType_Value, value);
	}

	callback(this->onToken, HTML_TokenType_Done, $(""));
}

def(void, Process, RdString s) {
	this->ofs = 0;
	this->buf = s;

	call(Parse);
}
