#import "Quirks.h"

#define self HTML_Quirks

rsdef(self, New, HTML_Tokenizer_OnToken onToken) {
	return (self) {
		.onToken   = onToken,
		.prev.type = HTML_Tokenizer_TokenType_Unset
	};
}

def(void, Destroy) { }

/* Non-nestable tags. */
static RdString tags[] = {
	$("area"),
	$("base"),
	$("br"),
	$("hr"),
	$("img"),
	$("input"),
	$("link"),
	$("meta"),
	$("param")
};

sdef(bool, Equals, RdString a, RdString b) {
	if (a.len != b.len) {
		return false;
	}

	fwd(i, a.len) {
		if (Char_ToLower(a.buf[i]) != Char_ToLower(b.buf[i])) {
			return false;
		}
	}

	return true;
}

def(void, ProcessToken, HTML_Tokenizer_TokenType type, RdString value) {
	if (type != HTML_Tokenizer_TokenType_Option   &&
		type != HTML_Tokenizer_TokenType_AttrName &&
		type != HTML_Tokenizer_TokenType_AttrValue)
	{
		bool inject = this->prev.type != HTML_Tokenizer_TokenType_Unset;

		if (type == HTML_Tokenizer_TokenType_TagEnd) {
			if (this->prev.type == HTML_Tokenizer_TokenType_TagEnd ||
				this->prev.type == HTML_Tokenizer_TokenType_TagStart)
			{
				inject = !scall(Equals, value, this->prevTag);
			} else {
				inject = false;
			}
		}

		if (inject) {
			fwd(i, nElems(tags)) {
				if (scall(Equals, tags[i], this->prevTag)) {
					callback(this->onToken,
						HTML_Tokenizer_TokenType_TagEnd, this->prevTag);
					break;
				}
			}
		}

		this->prevTag = $("");
	}

	if (type == HTML_Tokenizer_TokenType_TagStart) {
		this->prevTag = value;
	}

	this->prev.type  = type;
	this->prev.value = value;

	if (type == HTML_Tokenizer_TokenType_Done) {
		this->prev.type = HTML_Tokenizer_TokenType_Unset;
	}

	callback(this->onToken, type, value);
}
