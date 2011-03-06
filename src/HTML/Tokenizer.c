#import "Tokenizer.h"

#define self HTML_Tokenizer

def(void, Init, ref(OnToken) onToken) {
	this->buf      = String_New(100);
	this->curToken = String_New(100);
	this->onToken  = onToken;
	this->state    = 0;
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
				callback(this->onToken, ref(TokenType_Value), this->buf.prot);
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
			&& String_EndsWith(this->buf.prot, $("--"))
			&& c == '>') {
		String_Crop(&this->buf, 0, -2);
		callback(this->onToken, ref(TokenType_Comment), this->buf.prot);

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
	if (String_Equals(this->buf.prot, $("!--"))) {
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
		callback(this->onToken, ref(TokenType_AttrName), this->buf.prot);

		BitMask_Clear(this->state, ref(State_AttrName));
		BitMask_Set(this->state, ref(State_AttrValue));

		this->buf.len = 0;

		return;
	}

	/* Parse tokens, their attributes and options. */
	if (Char_IsSpace(c) || c == '/' || c == '>') {
		if (BitMask_Has(this->state, ref(State_TagName))) {
			if (this->buf.len > 0) {
				if (!String_BeginsWith(this->buf.prot, $("/"))) {
					/* Start of token. */
					callback(this->onToken,
						ref(TokenType_TagStart), this->buf.prot);

					/* For XHTML-ish tags. */
					String_Copy(&this->curToken, this->buf.prot);
				}
			}

			if (c != '/') {
				/* Expecting a tag name (true by default when a tag is
				 * introduced with `<'. */

				if (String_BeginsWith(this->buf.prot, $("/"))) {
					/* End of token. */
					if (this->buf.len > 1) {
						String_Crop(&this->buf, 1);
						callback(this->onToken,
							ref(TokenType_TagEnd), this->buf.prot);
					} else {
						callback(this->onToken,
							ref(TokenType_TagEnd), this->curToken.prot);
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
				ref(TokenType_AttrValue), this->buf.prot);

			BitMask_Set(this->state, ref(State_AttrName));
			BitMask_Clear(this->state, ref(State_AttrValue));
		} else if (String_Equals(this->buf.prot, $("/"))) {
			/* Create a fake `end'-token for XHTML tags such as `<br />'. */
			callback(this->onToken,
				ref(TokenType_TagEnd), this->curToken.prot);
		} else {
			/* Neither a name nor a value, thus we are assuming the current
			 * token to be an option such as `selected'. */

			ProtString cleaned = String_Trim(this->buf.prot);

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
		callback(this->onToken, ref(TokenType_Value), this->buf.prot);
	}
}

overload def(void, Process, String s) {
	for (size_t i = 0; i < s.len; i++) {
		call(ProcessChar, s.buf[i]);
	}

	call(Poll);
}

overload def(void, Process, StreamInterface *stream, void *context) {
	char c;

	while (stream->read(context, &c, 1) > 0) {
		call(ProcessChar, c);
	}

	call(Poll);
}
