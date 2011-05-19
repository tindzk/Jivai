#import <HTML/Entities.h>

#import "TestSuite.h"

#define self tsHTML_Entities

class {

};

tsRegister("HTML.Entities") {
	return true;
}

tsCase(Acute, "Decoding") {
	String s = HTML_Entities_Decode($("Hello"));
	Assert($("No entities"), String_Equals(s.rd, $("Hello")));
	String_Destroy(&s);

	s = HTML_Entities_Decode($("&amp;"));
	Assert($("Named entity"), String_Equals(s.rd, $("&")));
	String_Destroy(&s);

	s = HTML_Entities_Decode($("&#039;"));
	Assert($("Decimal entity"), String_Equals(s.rd, $("'")));
	String_Destroy(&s);

	s = HTML_Entities_Decode($("&oelig;"));
	Assert($("Named entity"), String_Equals(s.rd, $("œ")));
	String_Destroy(&s);

	s = HTML_Entities_Decode($("&#339;"));
	Assert($("Decimal entity"), String_Equals(s.rd, $("œ")));
	String_Destroy(&s);

	s = HTML_Entities_Decode($("&#x153;"));
	Assert($("Hex entity"), String_Equals(s.rd, $("œ")));
	String_Destroy(&s);
}

tsFinalize;
