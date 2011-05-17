#import <XML.h>

#import "TestSuite.h"

#define self tsXML

class {

};

tsRegister("XML") {
	return true;
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
