#import <Ecriture.h>

#import "TestSuite.h"

#define self tsEcriture

class {

};

tsRegister("Ecriture") {
	return true;
}

tsCase(Acute, "Unescaping") {
	CarrierString s = Ecriture_Unescape($("Hello World!"));
	Assert($("Equals"), String_Equals(s.rd, $("Hello World!")));
	Assert($("Stack"),  s.omni);
	CarrierString_Destroy(&s);

	s = Ecriture_Unescape($("Hello` World!"));
	Assert($("Equals"), String_Equals(s.rd, $("Hello World!")));
	Assert($("Heap"),   !s.omni);
	CarrierString_Destroy(&s);

	s = Ecriture_Unescape($("Hello`` World!"));
	Assert($("Equals"), String_Equals(s.rd, $("Hello` World!")));
	Assert($("Heap"),   !s.omni);
	CarrierString_Destroy(&s);

	s = Ecriture_Unescape($("Hello World!``"));
	Assert($("Equals"), String_Equals(s.rd, $("Hello World!`")));
	Assert($("Heap"),   !s.omni);
	CarrierString_Destroy(&s);

	s = Ecriture_Unescape($("Hello````World!"));
	Assert($("Equals"), String_Equals(s.rd, $("Hello``World!")));
	Assert($("Heap"),   !s.omni);
	CarrierString_Destroy(&s);
}

tsFinalize;
