#import "Builder.h"

#define self Ecriture_Builder

rsdef(self, New, Stream stream) {
	return (self) {
		.stream = stream
	};
}

def(void, Destroy) { }

def(void, Write, RdString str) {
	size_t written = delegate(this->stream, write, String_GetRdBuffer(str));
	assert(written == str.len);
}

def(void, ProcessToken, Ecriture_TokenType type, RdString value) {
	if (this->flush &&
		type != Ecriture_TokenType_Option   &&
		type != Ecriture_TokenType_AttrName &&
		type != Ecriture_TokenType_AttrValue)
	{
		call(Write, $("{"));
		this->flush = false;
	}

	if (type == Ecriture_TokenType_Value) {
		call(Write, value);
	} else if (type == Ecriture_TokenType_TagStart) {
		this->flush = true;
		call(Write, $("."));
		call(Write, value);
	} else if (type == Ecriture_TokenType_TagEnd) {
		call(Write, $("}"));
	} else if (type == Ecriture_TokenType_Comment) {
		call(Write, $("/*"));
		call(Write, value);
		call(Write, $("*/"));
	} else if (type == Ecriture_TokenType_Option) {
		call(Write, $("["));
		call(Write, value);
		call(Write, $("]"));
	} else if (type == Ecriture_TokenType_AttrName) {
		call(Write, $("["));
		call(Write, value);
		call(Write, $("="));
	} else if (type == Ecriture_TokenType_AttrValue) {
		call(Write, value);
		call(Write, $("]"));
	} else if (type == Ecriture_TokenType_Done) {
		this->flush = false;
	}
}
