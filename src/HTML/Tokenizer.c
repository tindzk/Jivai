#import "Tokenizer.h"

#define self HTML_Tokenizer

rsdef(self, New, ref(OnToken) onToken) {
	return (self) {
		.buf      = String_New(100),
		.curToken = String_New(100),
		.onToken  = onToken,
		.state    = 0
	};
}

def(void, Destroy) {
	String_Destroy(&this->buf);
	String_Destroy(&this->curToken);
}

def(void, Reset) {
	this->state = 0;

	this->buf.len = 0;
	this->curToken.len = 0;
}

def(void, ProcessChar, char c) {
	if (!BitMask_Has(this->state, ref(State_Tag))) {
		/* New tag begins? */
		if (c == '<') {
			/* Flush the value-buffer */
			if (this->buf.len != 0) {
				callback(this->onToken, ref(TokenType_Value), this->buf.rd);
				this->buf.len = 0;
			}

			BitMask_Set(this->state, ref(State_Tag));
			BitMask_Set(this->state, ref(State_TagName));

			return;
		}

		/* Buffer value */
		String_Append(&this->buf, c);

		return;
	}

	/* End of a comment */
	if (BitMask_Has(this->state, ref(State_Comment))
			&& String_EndsWith(this->buf.rd, $("--"))
			&& c == '>')
	{
		callback(this->onToken, ref(TokenType_Comment),
			String_Slice(this->buf.rd, 0, -2));

		BitMask_Clear(this->state, ref(State_Comment));
		BitMask_Clear(this->state, ref(State_Tag));

		this->buf.len = 0;

		return;
	}

	/* Within comment */
	if (BitMask_Has(this->state, ref(State_Comment))) {
		String_Append(&this->buf, c);

		return;
	}

	/* Start of a comment. */
	if (String_Equals(this->buf.rd, $("!--"))) {
		BitMask_Set(this->state, ref(State_Comment));

		this->buf.len = 0;
		String_Append(&this->buf, c);

		return;
	}

	if (BitMask_Has(this->state, ref(State_AttrValue))) {
		if (!BitMask_Has(this->state, ref(State_Quote))
		&& (c == '"' || c == '\'')) {
			this->curQuote = c;
			BitMask_Set(this->state, ref(State_Quote));
		} else if (BitMask_Has(this->state, ref(State_Quote))
				&& this->curQuote == c
				&& this->last != '\\') {
			BitMask_Clear(this->state, ref(State_Quote));
			String_Append(&this->buf, c);

			return;
		}
	}

	if (BitMask_Has(this->state, ref(State_Quote))) {
		String_Append(&this->buf, c);
		this->last = c;

		return;
	}

	if (BitMask_Has(this->state, ref(State_AttrName)) && c == '=') {
		/* Attribute name. */
		callback(this->onToken, ref(TokenType_AttrName), this->buf.rd);

		BitMask_Clear(this->state, ref(State_AttrName));
		BitMask_Set(this->state, ref(State_AttrValue));

		this->buf.len = 0;

		return;
	}

	/* Parse tokens, their attributes and options. */
	if (Char_IsSpace(c) || c == '/' || c == '>') {
		if (BitMask_Has(this->state, ref(State_TagName))) {
			if (this->buf.len > 0) {
				if (!String_BeginsWith(this->buf.rd, $("/"))) {
					/* Start of token. */
					callback(this->onToken,
						ref(TokenType_TagStart), this->buf.rd);

					/* For XHTML-ish tags. */
					String_Copy(&this->curToken, this->buf.rd);
				}
			}

			if (c != '/') {
				/* Expecting a tag name (true by default when a tag is
				 * introduced with `<'. */

				if (String_BeginsWith(this->buf.rd, $("/"))) {
					/* End of token. */
					if (this->buf.len > 1) {
						String_Crop(&this->buf, 1);
						callback(this->onToken,
							ref(TokenType_TagEnd), this->buf.rd);
					} else {
						callback(this->onToken,
							ref(TokenType_TagEnd), this->curToken.rd);
					}
				}

				if (Char_IsSpace(c)) {
					/* The token's end is not reached yet,
					 * hence we are expecting an attribute name. */
					BitMask_Set(this->state, ref(State_AttrName));
				}

				BitMask_Clear(this->state, ref(State_TagName));
			}
		} else if (BitMask_Has(this->state, ref(State_AttrValue))) {
			/* Expecting an attribute value. */

			HTML_Unescape(&this->buf);
			callback(this->onToken,
				ref(TokenType_AttrValue), this->buf.rd);

			BitMask_Set(this->state, ref(State_AttrName));
			BitMask_Clear(this->state, ref(State_AttrValue));
		} else if (String_Equals(this->buf.rd, $("/"))) {
			/* Create a fake `end'-token for XHTML tags such as `<br />'. */
			callback(this->onToken,
				ref(TokenType_TagEnd), this->curToken.rd);
		} else {
			/* Neither a name nor a value, thus we are assuming the current
			 * token to be an option such as `selected'. */

			RdString cleaned = String_Trim(this->buf.rd);

			if (cleaned.len > 0) {
				callback(this->onToken, ref(TokenType_Option), cleaned);
			}
		}

		this->buf.len = 0;
	}

	if (c == '>') {
		BitMask_Clear(this->state, ref(State_Tag));
		this->buf.len = 0;
	} else if (!Char_IsSpace(c)) {
		String_Append(&this->buf, c);
	}
}

/* Should be called after all characters have been processed.
 * It `flushes' the buffer. */
def(void, Poll) {
	if (this->buf.len != 0) {
		callback(this->onToken, ref(TokenType_Value), this->buf.rd);
	}
}

overload def(void, Process, RdString s) {
	fwd (i, s.len) {
		call(ProcessChar, s.buf[i]);
	}

	call(Poll);
}

overload def(void, Process, Stream stream) {
	char c;

	while (delegate(stream, read, Buffer_ForChar(&c)) > 0) {
		call(ProcessChar, c);
	}

	call(Poll);
}
