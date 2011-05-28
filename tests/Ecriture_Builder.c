#import <StringStream.h>
#import <Ecriture/Builder.h>

#import "TestSuite.h"

#define self tsEcriture_Builder

class {
	Ecriture_Builder html;
	String s;
	StringStream stream;
	bool clear;
};

tsRegister("Ecriture.Builder") {
	return true;
}

tsInit {
	this->s      = String_New(1024);
	this->stream = StringStream_New(&this->s);
	this->html   = Ecriture_Builder_New(StringStream_AsStream(&this->stream));
	this->clear  = false;
}

tsDestroy {
	Ecriture_Builder_Destroy(&this->html);
	String_Destroy(&this->s);
}

overload def(void, Process, Ecriture_TokenType type, RdString value) {
	if (this->clear) {
		StringStream_Clear(&this->stream);
		this->clear = false;
	}

	Ecriture_Builder_ProcessToken(&this->html, type, value);

	if (type == Ecriture_TokenType_Done) {
		this->clear = true;
	}
}

overload def(void, Process, Ecriture_TokenType type) {
	call(Process, type, $(""));
}

def(bool, Equals, RdString s) {
	return String_Equals(this->s.rd, s);
}

tsCase(Acute, "Value") {
	call(Process, Ecriture_TokenType_Value, $("  Hello World!  "));
	call(Process, Ecriture_TokenType_Done);

	Assert($("Equals"), call(Equals, $("  Hello World!  ")));

	call(Process, Ecriture_TokenType_Value, $("  "));
	call(Process, Ecriture_TokenType_Done);

	Assert($("Equals"), call(Equals, $("  ")));
}

tsCase(Acute, "Comments") {
	call(Process, Ecriture_TokenType_Comment, $(""));
	call(Process, Ecriture_TokenType_Done);

	Assert($("Equals"), call(Equals, $("/**/")));

	call(Process, Ecriture_TokenType_Comment, $("Comment"));
	call(Process, Ecriture_TokenType_Done);

	Assert($("Equals"), call(Equals, $("/*Comment*/")));

	call(Process, Ecriture_TokenType_Comment, $(" Comment "));
	call(Process, Ecriture_TokenType_Done);

	Assert($("Equals"), call(Equals, $("/* Comment */")));
}

tsCase(Acute, "Tags") {
	call(Process, Ecriture_TokenType_TagStart, $("br"));
	call(Process, Ecriture_TokenType_TagEnd, $(""));
	call(Process, Ecriture_TokenType_Done);

	Assert($("Equals"), call(Equals, $(".br{}")));

	call(Process, Ecriture_TokenType_TagStart, $("br"));
	call(Process, Ecriture_TokenType_Option, $("option"));
	call(Process, Ecriture_TokenType_Option, $("option2"));
	call(Process, Ecriture_TokenType_TagEnd, $(""));
	call(Process, Ecriture_TokenType_Done);

	Assert($("Equals"), call(Equals, $(".br[option][option2]{}")));

	call(Process, Ecriture_TokenType_TagStart, $("br"));
	call(Process, Ecriture_TokenType_Option, $("option"));
	call(Process, Ecriture_TokenType_Value, $("value"));
	call(Process, Ecriture_TokenType_TagEnd, $(""));
	call(Process, Ecriture_TokenType_Done);

	Assert($("Equals"), call(Equals, $(".br[option]{value}")));

	call(Process, Ecriture_TokenType_TagStart, $("br"));
	call(Process, Ecriture_TokenType_AttrName, $("name"));
	call(Process, Ecriture_TokenType_AttrValue, $("value"));
	call(Process, Ecriture_TokenType_AttrName, $("name2"));
	call(Process, Ecriture_TokenType_AttrValue, $("value2"));
	call(Process, Ecriture_TokenType_Value, $("value"));
	call(Process, Ecriture_TokenType_TagEnd, $(""));
	call(Process, Ecriture_TokenType_Done);

	Assert($("Equals"), call(Equals, $(".br[name=value][name2=value2]{value}")));
}

tsCase(Acute, "Nesting") {
	call(Process, Ecriture_TokenType_Value, $("value"));
	call(Process, Ecriture_TokenType_TagStart, $("p"));
	call(Process, Ecriture_TokenType_TagStart, $("b"));
	call(Process, Ecriture_TokenType_Value, $("parag"));
	call(Process, Ecriture_TokenType_TagEnd, $(""));
	call(Process, Ecriture_TokenType_TagEnd, $("p"));
	call(Process, Ecriture_TokenType_Done);

	Assert($("Equals"), call(Equals, $("value.p{.b{parag}}")));
}
