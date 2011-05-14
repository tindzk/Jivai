#import <StringStream.h>
#import <Ecriture/Parser.h>

#import "TestSuite.h"

#define self tsEcriture_Parser

record(ref(Element)) {
	Ecriture_TokenType type;
	String value;
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

def(void, OnToken, Ecriture_TokenType type, String value, size_t line);

tsInit {
	this->ecr = Ecriture_Parser_New(Ecriture_OnToken_For(this, ref(OnToken)));

	fwd(i, nElems(this->elements)) {
		this->elements[i] = (ref(Element)) { .value = String_New(0) };
	}

	this->cur   = 0;
	this->count = 0;
}

tsDestroy {
	fwd(i, this->count) {
		String_Destroy(&this->elements[i].value);
	}

	Ecriture_Parser_Destroy(&this->ecr);
}

def(void, OnToken, Ecriture_TokenType type, String value, __unused size_t line) {
	if (this->count != 0
		&& type == Ecriture_TokenType_Value
		&& this->elements[this->count - 1].type == Ecriture_TokenType_Value)
	{
		/* Merge values. */
		String_Append(&this->elements[this->count - 1].value, value.rd);
		String_Destroy(&value);
	} else {
		this->elements[this->count] = (ref(Element)) {
			.type  = type,
			.value = value
		};

		this->count++;
	}
}

def(bool, Matches, Ecriture_TokenType type, RdString value) {
	if (this->cur >= this->count) {
		return false;
	}

	this->cur++;

	if (type != this->elements[this->cur - 1].type) {
		return false;
	}

	if (!String_Equals(value, this->elements[this->cur - 1].value.rd)) {
		return false;
	}

	return true;
}

def(void, Process, RdString str) {
	fwd(i, this->count) {
		String_Destroy(&this->elements[i].value);
	}

	this->cur   = 0;
	this->count = 0;

	StringStream stream = StringStream_New(RdString_Exalt(str));
	Ecriture_Parser_Process(&this->ecr, StringStream_AsStream(&stream));
}

tsCase(Acute, "Value") {
	call(Process, $("  Hello World!  "));

	Assert($("Matches"), call(Matches, Ecriture_TokenType_Value, $("  Hello World!  ")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Done, $("")));

	call(Process, $("  "));

	Assert($("Matches"), call(Matches, Ecriture_TokenType_Value, $("  ")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Done, $("")));
}

tsCase(Acute, "Tags (simple)") {
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
}

tsCase(Acute, "Tags (extended)") {
	call(Process, $(".br[option]{}"));

	Assert($("Matches"), call(Matches, Ecriture_TokenType_TagStart, $("br")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Option, $("option")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_TagEnd, $("")));
	Assert($("Matches"), call(Matches, Ecriture_TokenType_Done, $("")));
}

tsFinalize;
