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

	s = HTML_Entities_Decode($("&#"));
	Assert($("No entities"), String_Equals(s.rd, $("&#")));
	String_Destroy(&s);

	s = HTML_Entities_Decode($("&#;"));
	Assert($("No entities"), String_Equals(s.rd, $("&#;")));
	String_Destroy(&s);

	s = HTML_Entities_Decode($("&abcd;"));
	Assert($("No entities"), String_Equals(s.rd, $("&abcd;")));
	String_Destroy(&s);

	s = HTML_Entities_Decode($("&#x;"));
	Assert($("Non-existent named entity"), String_Equals(s.rd, $("&#x;")));
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
