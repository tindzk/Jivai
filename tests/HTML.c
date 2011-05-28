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
