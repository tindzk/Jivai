#import <HTML.h>

#import "TestSuite.h"

#define self tsHTML

class {

};

tsRegister("HTML") {
	return true;
}

tsCase(Acute, "Escaping") {
	CarrierString s = HTML_Escape($("Hello"));
	Assert($("No quotes"), String_Equals(s.rd, $("Hello")));
	CarrierString_Destroy(&s);

	s = HTML_Escape($("Hello World!"));
	Assert($("No quotes"), String_Equals(s.rd, $("\"Hello World!\"")));
	CarrierString_Destroy(&s);

	s = HTML_Escape($("Hello' World!"));
	Assert($("Single quotes"), String_Equals(s.rd, $("\"Hello' World!\"")));
	CarrierString_Destroy(&s);

	s = HTML_Escape($("Hello\" World!"));
	Assert($("Double quotes"), String_Equals(s.rd, $("\"Hello\\\" World!\"")));
	CarrierString_Destroy(&s);

	s = HTML_Escape($("Hello'\" World!"));
	Assert($("Single and double quotes"), String_Equals(s.rd, $("\"Hello'\\\" World!\"")));
	CarrierString_Destroy(&s);
}

tsCase(Acute, "Unescaping") {
	CarrierString s = HTML_Unescape($("Hello World!"));
	Assert($("No quotes"), String_Equals(s.rd, $("Hello World!")));
	Assert($("Stack"),     s.omni);
	CarrierString_Destroy(&s);

	s = HTML_Unescape($("'Hello World!'"));
	Assert($("Single quotes"), String_Equals(s.rd, $("Hello World!")));
	Assert($("Stack"),         s.omni);
	CarrierString_Destroy(&s);

	s = HTML_Unescape($("\"Hello World!\""));
	Assert($("Double quotes"), String_Equals(s.rd, $("Hello World!")));
	Assert($("Stack"),         s.omni);
	CarrierString_Destroy(&s);

	s = HTML_Unescape($("'Hello\\' World!'"));
	Assert($("Escaped single quotes"), String_Equals(s.rd, $("Hello' World!")));
	Assert($("Heap"), !s.omni);
	CarrierString_Destroy(&s);

	s = HTML_Unescape($("'Hello\" World!'"));
	Assert($("Escaped single quotes"), String_Equals(s.rd, $("Hello\" World!")));
	Assert($("Stack"), s.omni);
	CarrierString_Destroy(&s);

	s = HTML_Unescape($("\"Hello\\\" World!\""));
	Assert($("Escaped double quotes"), String_Equals(s.rd, $("Hello\" World!")));
	Assert($("Heap"), !s.omni);
	CarrierString_Destroy(&s);

	s = HTML_Unescape($("\"Hello' World!\""));
	Assert($("Escaped double quotes"), String_Equals(s.rd, $("Hello' World!")));
	Assert($("Stack"), s.omni);
	CarrierString_Destroy(&s);

	s = HTML_Unescape($("'Hello\\'\" World!'"));
	Assert($("Mixing single and double quotes"), String_Equals(s.rd, $("Hello'\" World!")));
	Assert($("Heap"), !s.omni);
	CarrierString_Destroy(&s);

	s = HTML_Unescape($("\"Hello'\\\" World!\""));
	Assert($("Mixing single and double quotes"), String_Equals(s.rd, $("Hello'\" World!")));
	Assert($("Heap"), !s.omni);
	CarrierString_Destroy(&s);
}

tsFinalize;
