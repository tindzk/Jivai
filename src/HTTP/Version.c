#import "Version.h"

HTTP_Version HTTP_Version_FromString(String s) {
	if (String_Equals(s, String("HTTP/1.1"))) {
		return HTTP_Version_1_1;
	} else if (String_Equals(s, String("HTTP/1.0"))) {
		return HTTP_Version_1_0;
	}

	return HTTP_Version_Unset;
}
