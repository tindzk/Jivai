#import "Quirks.h"

#define self HTML_Quirks

rsdef(self, New, HTML_OnToken onToken) {
	return (self) {
		.onToken = onToken,
	};
}

def(void, Destroy) { }

def(void, ProcessToken, HTML_TokenType type, RdString value) {
	if (type == HTML_TokenType_TagEnd) {
		if (value.len == 0) {
			/* Refers to the last tag. */
			this->toClose = false;
		} else if (HTML_Equals(value, this->tagName)) {
			/* Current tag is already closed. */
			this->toClose = false;
		}
	}

	if (this->toClose && !HTML_IsTagAttr(type)) {
		callback(this->onToken, HTML_TokenType_TagEnd, $(""));
		this->toClose = false;
	}

	if (type == HTML_TokenType_TagStart) {
		this->tagName = value;
		if (!HTML_IsNestable(this->tagName)) {
			this->toClose = true;
		}
	}

	callback(this->onToken, type, value);
}
