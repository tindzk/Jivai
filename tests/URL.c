#import <URL.h>

#import "TestSuite.h"

#define self tsURL

class {

};

tsRegister("URL") {
	return true;
}

tsCase(Acute, "Parsing") {
	URL_Parts parts = URL_Parse($("http://www.google.com/info#main"));

	Assert($("Scheme"),   String_Equals(parts.scheme.rd,   $("http")));
	Assert($("Host"),     String_Equals(parts.host.rd,     $("www.google.com")));
	Assert($("Port"),     parts.port == 0);
	Assert($("Path"),     String_Equals(parts.path.rd,     $("/info")));
	Assert($("Fragment"), String_Equals(parts.fragment.rd, $("main")));

	URL_Parts_Destroy(&parts);
}

tsFinalize;
