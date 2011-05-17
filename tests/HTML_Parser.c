#import <HTML/Parser.h>

#import "TestSuite.h"

#define self tsHTML_Parser

record(ref(Element)) {
	XML_TokenType type;
	RdString value;
};

class {
	HTML_Parser html;
	ref(Element) elements[64];
	size_t cur;
	size_t count;
};

tsRegister("HTML.Parser") {
	return true;
}

def(void, OnToken, XML_TokenType type, RdString value);

tsInit {
	this->html = HTML_Parser_New(XML_OnToken_For(this, ref(OnToken)));

	fwd (i, nElems(this->elements)) {
		this->elements[i] = (ref(Element)) { .value = $("") };
	}

	this->cur   = 0;
	this->count = 0;
}

tsDestroy {
	HTML_Parser_Destroy(&this->html);
}

def(void, OnToken, XML_TokenType type, RdString value) {
	this->elements[this->count] = (ref(Element)) {
		.type  = type,
		.value = value
	};

	this->count++;
}

def(bool, Matches, XML_TokenType type, RdString value) {
	if (this->cur >= this->count) {
		return false;
	}

	this->cur++;

	if (type != this->elements[this->cur - 1].type) {
		return false;
	}

	if (!String_Equals(value, this->elements[this->cur - 1].value)) {
		return false;
	}

	return true;
}

def(void, Process, RdString str) {
	this->cur   = 0;
	this->count = 0;

	HTML_Parser_Process(&this->html, str);
}

tsCase(Acute, "Quirks") {
	call(Process, $("<br>"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("br")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagEnd, $("")));

	call(Process, $("<BR>"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("BR")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagEnd, $("")));

	call(Process, $("<br>test<br>"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("br")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagEnd, $("")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Value, $("test")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("br")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagEnd, $("")));

	call(Process, $("<br><input>"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("br")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagEnd, $("")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("input")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagEnd, $("")));

	call(Process, $("<br option>"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("br")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Option, $("option")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagEnd, $("")));

	call(Process, $("<meta />"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("meta")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagEnd, $("")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Done, $("")));

	call(Process, $("<meta /> <title>value</title>"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("meta")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagEnd, $("")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Value, $(" ")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("title")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Value, $("value")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagEnd, $("title")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Done, $("")));

	call(Process, $("<li><img></li>"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("li")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("img")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagEnd, $("")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagEnd, $("li")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Done, $("")));

	call(Process, $("<meta option />"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("meta")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Option, $("option")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagEnd, $("")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Done, $("")));

	call(Process, $("<li><img option></li>"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("li")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("img")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Option, $("option")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagEnd, $("")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagEnd, $("li")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Done, $("")));
}

tsFinalize;
