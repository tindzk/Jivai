#import <URL.h>

#import "TestSuite.h"

#define self tsURL

class {

};

tsRegister("URL") {
	return true;
}

tsCase(Acute, "Parsing") {
	URL_Parts parts = URL_Parse($("http://localhost"));

	Assert($("Scheme"),   String_Equals(parts.scheme,   $("http")));
	Assert($("Host"),     String_Equals(parts.host,     $("localhost")));
	Assert($("Port"),     parts.port == 0);
	Assert($("Path"),     String_Equals(parts.path,     $("")));
	Assert($("Fragment"), String_Equals(parts.fragment, $("")));
}

tsCase(Acute, "Parsing") {
	URL_Parts parts = URL_Parse($("http://localhost/"));

	Assert($("Scheme"),   String_Equals(parts.scheme,   $("http")));
	Assert($("Host"),     String_Equals(parts.host,     $("localhost")));
	Assert($("Port"),     parts.port == 0);
	Assert($("Path"),     String_Equals(parts.path,     $("/")));
	Assert($("Fragment"), String_Equals(parts.fragment, $("")));
}

tsCase(Acute, "Parsing") {
	URL_Parts parts = URL_Parse($("http://localhost:80/"));

	Assert($("Scheme"),   String_Equals(parts.scheme,   $("http")));
	Assert($("Host"),     String_Equals(parts.host,     $("localhost")));
	Assert($("Port"),     parts.port == 80);
	Assert($("Path"),     String_Equals(parts.path,     $("/")));
	Assert($("Fragment"), String_Equals(parts.fragment, $("")));
}

tsCase(Acute, "Parsing") {
	URL_Parts parts = URL_Parse($("http://localhost:80/info"));

	Assert($("Scheme"),   String_Equals(parts.scheme,   $("http")));
	Assert($("Host"),     String_Equals(parts.host,     $("localhost")));
	Assert($("Port"),     parts.port == 80);
	Assert($("Path"),     String_Equals(parts.path,     $("/info")));
	Assert($("Fragment"), String_Equals(parts.fragment, $("")));
}

tsCase(Acute, "Parsing") {
	URL_Parts parts = URL_Parse($("http://localhost:80/info#main"));

	Assert($("Scheme"),   String_Equals(parts.scheme,   $("http")));
	Assert($("Host"),     String_Equals(parts.host,     $("localhost")));
	Assert($("Port"),     parts.port == 80);
	Assert($("Path"),     String_Equals(parts.path,     $("/info")));
	Assert($("Fragment"), String_Equals(parts.fragment, $("main")));
}

tsCase(Acute, "Parsing") {
	URL_Parts parts = URL_Parse($("http://localhost:80/info?qry#main"));

	Assert($("Scheme"),   String_Equals(parts.scheme,   $("http")));
	Assert($("Host"),     String_Equals(parts.host,     $("localhost")));
	Assert($("Port"),     parts.port == 80);
	Assert($("Path"),     String_Equals(parts.path,     $("/info")));
	Assert($("Query"),    String_Equals(parts.query,    $("qry")));
	Assert($("Fragment"), String_Equals(parts.fragment, $("main")));
}

tsFinalize;
