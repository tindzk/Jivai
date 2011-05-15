#import "Parser.h"

#define self HTML_Parser

rsdef(self, New, HTML_OnToken onToken) {
	return (self) {
		.onToken = onToken
	};
}

def(void, Destroy) { }

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
	while (StringReader_Peek(&this->reader, &c)) {
		if (!Char_IsSpace(c)) {
			break;
		}

		StringReader_Consume(&this->reader);
	}

	while (StringReader_Peek(&this->reader, &c)) {
		if (quote) {
			StringReader_Extend(&this->reader, &value);

			/* As for quoted values, stop as soon as the value is complete. */
			if (c == quoteType && prev != '\\') {
				callback(this->onToken, HTML_TokenType_AttrValue, value);
				break;
			}
		} else if (c == '"' || c == '\'') {
			/* Handle quoted values ("value", 'value'). */
			quote     = true;
			quoteType = c;

			StringReader_Extend(&this->reader, &value);
		} else if (Char_IsSpace(c) || c == '>') {
			/* As for unquoted values, we don't have a clear end delimiter
			 * (like ' or "). We'll stop processing after a space or ">".
			 */

			callback(this->onToken, HTML_TokenType_AttrValue, value);
			break;
		} else {
			/* Unquoted value. */
			StringReader_Extend(&this->reader, &value);
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
	while (StringReader_Peek(&this->reader, &c)) {
		if (!Char_IsSpace(c)) {
			break;
		}

		StringReader_Consume(&this->reader);
	}

	bool space = false;

	while (StringReader_Peek(&this->reader, &c)) {
		if (Char_IsSpace(c)) {
			space = true;
		}

		if (c == '=') {
			callback(this->onToken, HTML_TokenType_AttrName, name);
			StringReader_Consume(&this->reader);

			call(ParseAttrValue);
			return true;
		} else if (c == '>'
				|| (StringReader_Peek(&this->reader, &str, 2) && String_Equals(str, $("/>"))))
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

			StringReader_Consume(&this->reader);
		} else {
			StringReader_Extend(&this->reader, &name);
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

	while (StringReader_Peek(&this->reader, &c)) {
		if (c == '>') {
			StringReader_Consume(&this->reader);

			if (commitName) {
				callback(this->onToken, HTML_TokenType_TagStart, name);
			}
			break;
		} else if (StringReader_Peek(&this->reader, &str, 2) && String_Equals(str, $("/>"))) {
			/* This is an XHTML tag like `<br />'. */
			StringReader_Consume(&this->reader, 2);

			if (commitName) {
				callback(this->onToken, HTML_TokenType_TagStart, name);
			}

			callback(this->onToken, HTML_TokenType_TagEnd, $(""));
			break;
		} else if (Char_IsSpace(c)) {
			StringReader_Consume(&this->reader);

			if (commitName) {
				callback(this->onToken, HTML_TokenType_TagStart, name);
				commitName = false;
			}

			/* Parse all attributes until the tag has reached its end. */
			while(call(ParseAttr));
		} else {
			StringReader_Extend(&this->reader, &name);
		}
	}
}

/* Matches "tagName>". */
def(void, ParseTagEnd) {
	char c;
	RdString name = $("");

	while (StringReader_Peek(&this->reader, &c)) {
		if (c == '>') {
			StringReader_Consume(&this->reader);
			callback(this->onToken, HTML_TokenType_TagEnd, name);
			break;
		} else {
			StringReader_Extend(&this->reader, &name);
		}
	}
}

/* Matches "...-->". */
def(void, ParseComment) {
	RdString str;
	RdString comment = $("");

	while (!StringReader_IsEnd(&this->reader)) {
		if (StringReader_Peek(&this->reader, &str, 3) && String_Equals(str, $("-->"))) {
			StringReader_Consume(&this->reader, 3);
			callback(this->onToken, HTML_TokenType_Comment, comment);
			break;
		} else {
			StringReader_Extend(&this->reader, &comment);
		}
	}
}

/* Matches "...]]>". */
def(void, ParseData) {
	RdString str;
	RdString data = $("");

	while (!StringReader_IsEnd(&this->reader)) {
		if (StringReader_Peek(&this->reader, &str, 3) && String_Equals(str, $("]]>"))) {
			StringReader_Consume(&this->reader, 3);
			callback(this->onToken, HTML_TokenType_Data, data);
			break;
		} else {
			StringReader_Extend(&this->reader, &data);
		}
	}
}

/* Matches "...>". */
def(void, ParseType) {
	char c;
	RdString type = $("");

	while (StringReader_Peek(&this->reader, &c)) {
		if (c == '>') {
			StringReader_Consume(&this->reader);
			callback(this->onToken, HTML_TokenType_Type, type);
			break;
		} else {
			StringReader_Extend(&this->reader, &type);
		}
	}
}

/* Matches "...", "/..." and "!--...". */
def(void, ParseTag) {
	char c;
	RdString str;
	StringReader_Peek(&this->reader, &c);

	if (c == '/') {
		StringReader_Consume(&this->reader);
		call(ParseTagEnd);
	} else if (StringReader_Peek(&this->reader, &str, 8) && String_Equals(str, $("![CDATA["))) {
		StringReader_Consume(&this->reader, 8);
		call(ParseData);
	} else if (StringReader_Peek(&this->reader, &str, 9) && String_Equals(str, $("!DOCTYPE "))) {
		StringReader_Consume(&this->reader, 9);
		call(ParseType);
	} else if (StringReader_Peek(&this->reader, &str, 3) && String_Equals(str, $("!--"))) {
		StringReader_Consume(&this->reader, 3);
		call(ParseComment);
	} else {
		call(ParseTagStart);
	}
}

/* Allows loose and invalid `<'s. */
def(bool, IsTag) {
	RdString str;

	if (StringReader_Peek(&this->reader, &str, 2)) {
		if (str.buf[0] == '<') {
			if (str.buf[1] == '!' || str.buf[1] == '/' || Char_IsAlpha(str.buf[1])) {
				return true;
			}
		}
	}

	return false;
}

/* Matches "..." and "<...". */
static def(void, Parse) {
	char c;
	RdString value = $("");

	while (StringReader_Peek(&this->reader, &c)) {
		if (!call(IsTag)) {
			StringReader_Extend(&this->reader, &value);
		} else {
			StringReader_Consume(&this->reader);

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
	this->reader = StringReader_New(s);
	call(Parse);
}
