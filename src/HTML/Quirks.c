#import "Quirks.h"

#define self HTML_Quirks

rsdef(self, New, HTML_OnToken onToken) {
	return (self) {
		.onToken = onToken,
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

def(void, ProcessToken, HTML_TokenType type, RdString value) {
	if (type == HTML_TokenType_TagEnd) {
		if (value.len == 0) {
			/* Refers to the last tag. */
			this->toClose = false;
		} else if (scall(Equals, value, this->tagName)) {
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
		fwd(i, nElems(tags)) {
			if (scall(Equals, tags[i], this->tagName)) {
				this->toClose = true;
				break;
			}
		}
	}

	callback(this->onToken, type, value);
}
