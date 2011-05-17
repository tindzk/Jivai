#import <StringStream.h>
#import <HTML/Builder.h>

#import "TestSuite.h"

#define self tsHTML_Builder

class {
	HTML_Builder html;
	String s;
	StringStream stream;
	bool clear;
};

tsRegister("HTML.Builder") {
	return true;
}

tsInit {
	this->s      = String_New(1024);
	this->stream = StringStream_New(&this->s);
	this->html   = HTML_Builder_New(StringStream_AsStream(&this->stream));
	this->clear  = false;
}

tsDestroy {
	HTML_Builder_Destroy(&this->html);
	String_Destroy(&this->s);
}

overload def(void, Process, HTML_TokenType type, RdString value) {
	if (this->clear) {
		StringStream_Clear(&this->stream);
		this->clear = false;
	}

	HTML_Builder_ProcessToken(&this->html, type, value);

	if (type == HTML_TokenType_Done) {
		this->clear = true;
	}
}

overload def(void, Process, HTML_TokenType type) {
	call(Process, type, $(""));
}

def(bool, Equals, RdString s) {
	return String_Equals(this->s.rd, s);
}

tsCase(Acute, "Value") {
	call(Process, HTML_TokenType_Value, $("  Hello World!  "));
	call(Process, HTML_TokenType_Done);

	Assert($("Equals"), call(Equals, $("  Hello World!  ")));

	call(Process, HTML_TokenType_Value, $("  "));
	call(Process, HTML_TokenType_Done);

	Assert($("Equals"), call(Equals, $("  ")));
}

tsCase(Acute, "Comments") {
	call(Process, HTML_TokenType_Comment, $(""));
	call(Process, HTML_TokenType_Done);

	Assert($("Equals"), call(Equals, $("<!---->")));

	call(Process, HTML_TokenType_Comment, $("Comment"));
	call(Process, HTML_TokenType_Done);

	Assert($("Equals"), call(Equals, $("<!--Comment-->")));

	call(Process, HTML_TokenType_Comment, $(" Comment "));
	call(Process, HTML_TokenType_Done);

	Assert($("Equals"), call(Equals, $("<!-- Comment -->")));

	call(Process, HTML_TokenType_Comment, $("[if IE]><link rel=\"stylesheet\" /><![endif]"));
	call(Process, HTML_TokenType_Done);

	Assert($("Equals"), call(Equals, $("<!--[if IE]><link rel=\"stylesheet\" /><![endif]-->")));
}

tsCase(Acute, "Tags (simple)") {
	call(Process, HTML_TokenType_TagStart, $("br"));
	call(Process, HTML_TokenType_TagEnd, $("br"));
	call(Process, HTML_TokenType_Done);

	Assert($("Equals"), call(Equals, $("<br></br>")));

	call(Process, HTML_TokenType_TagStart, $("br"));
	call(Process, HTML_TokenType_Option, $("option"));
	call(Process, HTML_TokenType_TagEnd, $("br"));
	call(Process, HTML_TokenType_Done);

	Assert($("Equals"), call(Equals, $("<br option></br>")));
}

tsCase(Acute, "Tags (XHTML)") {
	call(Process, HTML_TokenType_TagStart, $("br"));
	call(Process, HTML_TokenType_TagEnd);
	call(Process, HTML_TokenType_Done);

	Assert($("Equals"), call(Equals, $("<br />")));

	call(Process, HTML_TokenType_TagStart, $("br"));
	call(Process, HTML_TokenType_Option, $("option"));
	call(Process, HTML_TokenType_TagEnd);
	call(Process, HTML_TokenType_Done);

	Assert($("Equals"), call(Equals, $("<br option />")));
}

tsCase(Acute, "Attributes") {
	call(Process, HTML_TokenType_TagStart, $("input"));
	call(Process, HTML_TokenType_AttrName, $("name"));
	call(Process, HTML_TokenType_AttrValue, $("text"));
	call(Process, HTML_TokenType_TagEnd);
	call(Process, HTML_TokenType_Done);

	Assert($("Equals"), call(Equals, $("<input name=text />")));

	call(Process, HTML_TokenType_TagStart, $("input"));
	call(Process, HTML_TokenType_AttrName, $("name"));
	call(Process, HTML_TokenType_AttrValue, $("text"));
	call(Process, HTML_TokenType_TagEnd, $("input"));
	call(Process, HTML_TokenType_Done);

	Assert($("Equals"), call(Equals, $("<input name=text></input>")));

	call(Process, HTML_TokenType_TagStart, $("input"));
	call(Process, HTML_TokenType_AttrName, $("name"));
	call(Process, HTML_TokenType_AttrValue, $("text"));
	call(Process, HTML_TokenType_Value, $("Value"));
	call(Process, HTML_TokenType_TagEnd, $("input"));
	call(Process, HTML_TokenType_Done);

	Assert($("Equals"), call(Equals, $("<input name=text>Value</input>")));
}

tsCase(Acute, "Nesting") {
	call(Process, HTML_TokenType_Value, $("value"));
	call(Process, HTML_TokenType_TagStart, $("p"));
	call(Process, HTML_TokenType_TagStart, $("b"));
	call(Process, HTML_TokenType_Value, $("parag"));
	call(Process, HTML_TokenType_TagEnd, $("b"));
	call(Process, HTML_TokenType_TagEnd, $("p"));
	call(Process, HTML_TokenType_Done);

	Assert($("Equals"), call(Equals, $("value<p><b>parag</b></p>")));
}

tsFinalize;
