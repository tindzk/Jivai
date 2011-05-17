#import "Parser.h"

#define self HTML_Parser

rsdef(self, New, XML_OnToken onToken) {
	return (self) {
		.onToken = onToken
	};
}

def(void, Destroy) { }

static def(void, ProcessToken, XML_TokenType type, RdString value) {
	if (type == XML_TokenType_TagEnd) {
		if (value.len == 0) {
			/* Refers to the last tag. */
			this->toClose = false;
		} else if (HTML_Equals(value, this->tagName)) {
			/* Current tag is already closed. */
			this->toClose = false;
		}
	}

	if (this->toClose && !XML_IsTagAttr(type)) {
		callback(this->onToken, XML_TokenType_TagEnd, $(""));
		this->toClose = false;
	}

	if (type == XML_TokenType_TagStart) {
		this->tagName = value;
		if (!HTML_IsNestable(this->tagName)) {
			this->toClose = true;
		}
	}

	callback(this->onToken, type, value);
}

def(void, Initialize) {
	this->parser = XML_Parser_New(XML_OnToken_For(this, ref(ProcessToken)));
}

def(void, Process, RdString s) {
	XML_Parser_Process(&this->parser, s);
}
