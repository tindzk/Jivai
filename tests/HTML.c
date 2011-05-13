#import <HTML.h>

#import "TestSuite.h"

#define self tsHTML

class {

};

tsRegister("HTML") {
	return true;
}

tsCase(Acute, "Escaping") {
	String s = HTML_Escape($("Hello World!"));
	Assert($("No quotes"), String_Equals(s.rd, $("\"Hello World!\"")));
	String_Destroy(&s);

	s = HTML_Escape($("Hello' World!"));
	Assert($("Single quotes"), String_Equals(s.rd, $("\"Hello' World!\"")));
	String_Destroy(&s);

	s = HTML_Escape($("Hello\" World!"));
	Assert($("Double quotes"), String_Equals(s.rd, $("\"Hello\\\" World!\"")));
	String_Destroy(&s);

	s = HTML_Escape($("Hello'\" World!"));
	Assert($("Single and double quotes"), String_Equals(s.rd, $("\"Hello'\\\" World!\"")));
	String_Destroy(&s);
}

tsCase(Acute, "Unescaping") {
	String s = HTML_Unescape($("'Hello World!'"));
	Assert($("Single quotes"), String_Equals(s.rd, $("Hello World!")));
	String_Destroy(&s);

	s = HTML_Unescape($("\"Hello World!\""));
	Assert($("Double quotes"), String_Equals(s.rd, $("Hello World!")));
	String_Destroy(&s);

	s = HTML_Unescape($("'Hello\\' World!'"));
	Assert($("Escaped single quotes"), String_Equals(s.rd, $("Hello' World!")));
	String_Destroy(&s);

	s = HTML_Unescape($("'Hello\" World!'"));
	Assert($("Escaped single quotes"), String_Equals(s.rd, $("Hello\" World!")));
	String_Destroy(&s);

	s = HTML_Unescape($("\"Hello\\\" World!\""));
	Assert($("Escaped double quotes"), String_Equals(s.rd, $("Hello\" World!")));
	String_Destroy(&s);

	s = HTML_Unescape($("\"Hello' World!\""));
	Assert($("Escaped double quotes"), String_Equals(s.rd, $("Hello' World!")));
	String_Destroy(&s);

	s = HTML_Unescape($("'Hello\\'\" World!'"));
	Assert($("Mixing single and double quotes"), String_Equals(s.rd, $("Hello'\" World!")));
	String_Destroy(&s);

	s = HTML_Unescape($("\"Hello'\\\" World!\""));
	Assert($("Mixing single and double quotes"), String_Equals(s.rd, $("Hello'\" World!")));
	String_Destroy(&s);
}

tsFinalize;
