#include "Tokenizer.h"

void HTML_Tokenizer_Init(HTML_Tokenizer *this, HTML_Tokenizer_OnToken onToken, void *context) {
	this->onToken = onToken;
	this->context = context;

	this->state = 0;

	this->buf      = HeapString(100);
	this->curToken = HeapString(100);
}

void HTML_Tokenizer_Destroy(HTML_Tokenizer *this) {
	String_Destroy(&this->buf);
	String_Destroy(&this->curToken);
}

void HTML_Tokenizer_Reset(HTML_Tokenizer *this) {
	this->state = 0;

	this->buf.len = 0;
	this->curToken.len = 0;
}

void HTML_Tokenizer_ProcessChar(HTML_Tokenizer *this, char c) {
	if (!BitMask_Has(this->state, HTML_Tokenizer_State_Tag)) {
		/* New tag begins? */
		if (c == '<') {
			/* Flush the value-buffer */
			if (this->buf.len != 0) {
				this->onToken(this->context, HTML_Tokenizer_TokenType_Value, this->buf);
				this->buf.len = 0;
			}

			BitMask_Set(this->state, HTML_Tokenizer_State_Tag);
			BitMask_Set(this->state, HTML_Tokenizer_State_TagName);

			return;
		}

		/* Buffer value */
		String_Append(&this->buf, c);

		return;
	}

	/* End of a comment */
	if (BitMask_Has(this->state, HTML_Tokenizer_State_Comment)
			&& String_EndsWith(&this->buf, String("--"))
			&& c == '>') {
		String_Crop(&this->buf, 0, -2);
		this->onToken(this->context, HTML_Tokenizer_TokenType_Comment, this->buf);

		BitMask_Clear(this->state, HTML_Tokenizer_State_Comment);
		BitMask_Clear(this->state, HTML_Tokenizer_State_Tag);

		this->buf.len = 0;

		return;
	}

	/* Within comment */
	if (BitMask_Has(this->state, HTML_Tokenizer_State_Comment)) {
		String_Append(&this->buf, c);

		return;
	}

	/* Start of a comment. */
	if (String_Equals(&this->buf, String("!--"))) {
		BitMask_Set(this->state, HTML_Tokenizer_State_Comment);

		this->buf.len = 0;
		String_Append(&this->buf, c);

		return;
	}

	if (BitMask_Has(this->state, HTML_Tokenizer_State_AttrValue)) {
		if (!BitMask_Has(this->state, HTML_Tokenizer_State_Quote)
		&& (c == '"' || c == '\'')) {
			this->curQuote = c;
			BitMask_Set(this->state, HTML_Tokenizer_State_Quote);
		} else if (BitMask_Has(this->state, HTML_Tokenizer_State_Quote)
				&& this->curQuote == c
				&& this->last != '\\') {
			BitMask_Clear(this->state, HTML_Tokenizer_State_Quote);
			String_Append(&this->buf, c);

			return;
		}
	}

	if (BitMask_Has(this->state, HTML_Tokenizer_State_Quote)) {
		String_Append(&this->buf, c);
		this->last = c;

		return;
	}

	if (BitMask_Has(this->state, HTML_Tokenizer_State_AttrName) && c == '=') {
		/* Attribute name. */
		this->onToken(this->context, HTML_Tokenizer_TokenType_AttrName, this->buf);

		BitMask_Clear(this->state, HTML_Tokenizer_State_AttrName);
		BitMask_Set(this->state, HTML_Tokenizer_State_AttrValue);

		this->buf.len = 0;

		return;
	}

	/* Parse tokens, their attributes and options. */
	if (Char_IsSpace(c) || c == '/' || c == '>') {
		if (BitMask_Has(this->state, HTML_Tokenizer_State_TagName)) {
			if (this->buf.len > 0) {
				if (!String_BeginsWith(&this->buf, String("/"))) {
					/* Start of token. */
					this->onToken(this->context, HTML_Tokenizer_TokenType_TagStart, this->buf);

					/* For XHTML-ish tags. */
					String_Copy(&this->curToken, this->buf);
				}
			}

			if (c != '/') {
				/* Expecting a tag name (true by default when a tag is
				 * introduced with `<'. */

				if (String_BeginsWith(&this->buf, String("/"))) {
					/* End of token. */
					if (this->buf.len > 1) {
						String_Crop(&this->buf, 1);
						this->onToken(this->context, HTML_Tokenizer_TokenType_TagEnd, this->buf);
					} else {
						this->onToken(this->context, HTML_Tokenizer_TokenType_TagEnd, this->curToken);
					}
				}

				if (Char_IsSpace(c)) {
					/* The token's end is not reached yet,
					 * hence we are expecting an attribute name. */
					BitMask_Set(this->state, HTML_Tokenizer_State_AttrName);
				}

				BitMask_Clear(this->state, HTML_Tokenizer_State_TagName);
			}
		} else if (BitMask_Has(this->state, HTML_Tokenizer_State_AttrValue)) {
			/* Expecting an attribute value. */

			HTML_Unescape(&this->buf);
			this->onToken(this->context, HTML_Tokenizer_TokenType_AttrValue, this->buf);

			BitMask_Set(this->state, HTML_Tokenizer_State_AttrName);
			BitMask_Clear(this->state, HTML_Tokenizer_State_AttrValue);
		} else if (String_Equals(&this->buf, String("/"))) {
			/* Create a fake `end'-token for XHTML tags such as `<br />'. */
			this->onToken(this->context, HTML_Tokenizer_TokenType_TagEnd, this->curToken);
		} else {
			/* Neither a name nor a value, thus we are assuming the current
			 * token to be an option such as `selected'. */

			String_Trim(&this->buf);

			if (this->buf.len > 0) {
				this->onToken(this->context, HTML_Tokenizer_TokenType_Option, this->buf);
			}
		}

		this->buf.len = 0;
	}

	if (c == '>') {
		BitMask_Clear(this->state, HTML_Tokenizer_State_Tag);
		this->buf.len = 0;
	} else if (!Char_IsSpace(c)) {
		String_Append(&this->buf, c);
	}
}

/* Should be called after all characters have been processed.
 * It `flushes' the buffer. */
void HTML_Tokenizer_Poll(HTML_Tokenizer *this) {
	if (this->buf.len != 0) {
		this->onToken(this->context, HTML_Tokenizer_TokenType_Value, this->buf);
	}
}

void HTML_Tokenizer_ProcessString(HTML_Tokenizer *this, String s) {
	for (size_t i = 0; i < s.len; i++) {
		HTML_Tokenizer_ProcessChar(this, s.buf[i]);
	}

	HTML_Tokenizer_Poll(this);
}

void HTML_Tokenizer_ProcessStream(HTML_Tokenizer *this, StreamInterface *stream, void *context) {
	char c;

	while (stream->read(context, &c, 1) > 0) {
		HTML_Tokenizer_ProcessChar(this, c);
	}

	HTML_Tokenizer_Poll(this);
}
