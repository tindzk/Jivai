#import <Ecriture/Parser.h>

#import "TestSuite.h"

#define self tsEcriture_Parser

record(ref(Element)) {
	Ecriture_TokenType type;
	RdString value;
};

class {
	Ecriture_Parser ecr;
	ref(Element) elements[64];
	size_t cur;
	size_t count;
};

tsRegister("Ecriture.Parser") {
	return true;
}

def(void, OnToken, Ecriture_TokenType type, RdString value, size_t line);

tsInit {
	this->ecr = Ecriture_Parser_New(Ecriture_OnToken_For(this, ref(OnToken)));

	fwd(i, nElems(this->elements)) {
		this->elements[i] = (ref(Element)) { .value = $("") };
	}

	this->cur   = 0;
	this->count = 0;
}

tsDestroy {
	Ecriture_Parser_Destroy(&this->ecr);
}

def(void, OnToken, Ecriture_TokenType type, RdString value, __unused size_t line) {
	this->elements[this->count] = (ref(Element)) {
		.type  = type,
		.value = value
	};

	this->count++;
}

def(bool, Matches, Ecriture_TokenType type, RdString value) {
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

	Ecriture_Parser_Process(&this->ecr, str);
}

tsCase(Acute, "Values") {
	call(Process, $("  Hello World!  "));

	Assert($("Matches"), call(Matches, Ecriture_TokenType_Value, $("  Hello World!  ")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Done, $("")));

	call(Process, $("  "));

	Assert($("Matches"), call(Matches, Ecriture_TokenType_Value, $("  ")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Done, $("")));
}

tsCase(Acute, "Tags") {
	call(Process, $(".br{}"));

	Assert($("Matches"), call(Matches, Ecriture_TokenType_TagStart, $("br")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_TagEnd, $("")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Done, $("")));

	call(Process, $(".br{Value}"));

	Assert($("Matches"), call(Matches, Ecriture_TokenType_TagStart, $("br")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Value, $("Value")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_TagEnd, $("")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Done, $("")));

	call(Process, $("Value .br{}"));

	Assert($("Matches"), call(Matches, Ecriture_TokenType_Value, $("Value ")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_TagStart, $("br")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_TagEnd, $("")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Done, $("")));

	call(Process, $("Value....br{}"));

	Assert($("Matches"), call(Matches, Ecriture_TokenType_Value, $("Value...")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_TagStart, $("br")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_TagEnd, $("")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Done, $("")));

	call(Process, $("start .br{} end"));

	Assert($("Matches"), call(Matches, Ecriture_TokenType_Value, $("start ")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_TagStart, $("br")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_TagEnd, $("")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Value, $(" end")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Done, $("")));
}

tsCase(Acute, "Options") {
	call(Process, $(".br[option]{}"));

	Assert($("Matches"), call(Matches, Ecriture_TokenType_TagStart, $("br")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Option, $("option")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_TagEnd, $("")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Done, $("")));

	call(Process, $(".br[option][option2]{}"));

	Assert($("Matches"), call(Matches, Ecriture_TokenType_TagStart, $("br")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Option, $("option")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Option, $("option2")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_TagEnd, $("")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Done, $("")));
}

tsCase(Acute, "Escaping") {
	call(Process, $(".br[op`]tion][option2`]]{}"));

	Assert($("Matches"), call(Matches, Ecriture_TokenType_TagStart, $("br")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Option, $("op`]tion")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Option, $("option2`]")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_TagEnd, $("")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Done, $("")));

	call(Process, $("`.br{}`"));

	Assert($("Matches"), call(Matches, Ecriture_TokenType_Literal, $(".br{}")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Done, $("")));

	call(Process, $("`.br{}```"));

	Assert($("Matches"), call(Matches, Ecriture_TokenType_Literal, $(".br{}``")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Done, $("")));

	call(Process, $("`.br[op``]tion][option2``]]{}`"));

	Assert($("Matches"), call(Matches, Ecriture_TokenType_Literal,
		$(".br[op``]tion][option2``]]{}")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Done, $("")));
}

tsCase(Acute, "Nesting") {
	call(Process, $("a .fg[blue]{b .b{c .i{d}}}e"));

	Assert($("Matches"), call(Matches, Ecriture_TokenType_Value, $("a ")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_TagStart, $("fg")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Option, $("blue")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Value, $("b ")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_TagStart, $("b")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Value, $("c ")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_TagStart, $("i")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Value, $("d")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_TagEnd, $("")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_TagEnd, $("")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_TagEnd, $("")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Value, $("e")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Done, $("")));
}

tsFinalize;
