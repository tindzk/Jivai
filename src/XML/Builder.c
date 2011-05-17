#import "Builder.h"

#define self XML_Builder

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

def(void, ProcessToken, XML_TokenType type, RdString value) {
	if (type == XML_TokenType_TagEnd) {
		if (value.len == 0) {
			call(Write, $("/"));
		}
	}

	if (this->mustClose && !XML_IsTagAttr(type)) {
		call(Write, $(">"));
		this->mustClose = false;
	}

	if (type == XML_TokenType_Type) {
		call(Write, $("<!DOCTYPE "));
		call(Write, value);
		call(Write, $("]]>"));
	} else if (type == XML_TokenType_Data) {
		call(Write, $("<![CDATA["));
		call(Write, value);
		call(Write, $("]]>"));
	} else if (type == XML_TokenType_Value) {
		call(Write, value);
	} else if (type == XML_TokenType_TagStart) {
		call(Write, $("<"));
		call(Write, value);
		this->mustClose = true;
	} else if (type == XML_TokenType_TagEnd) {
		if (value.len != 0) {
			call(Write, $("</"));
			call(Write, value);
			call(Write, $(">"));
		}
	} else if (type == XML_TokenType_Comment) {
		call(Write, $("<!--"));
		call(Write, value);
		call(Write, $("-->"));
	} else if (type == XML_TokenType_AttrName) {
		call(Write, $(" "));
		call(Write, value);
	} else if (type == XML_TokenType_AttrValue) {
		call(Write, $("="));
		call(Write, value);
	} else if (type == XML_TokenType_Option) {
		call(Write, $(" "));
		call(Write, value);
	} else if (type == XML_TokenType_Done) {
		this->mustClose = false;
	}
}
