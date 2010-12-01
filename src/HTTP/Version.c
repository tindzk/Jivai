#import "Version.h"

#define self HTTP_Version

sdef(self, FromString, String s) {
	if (String_Equals(s, $("HTTP/1.1"))) {
		return ref(1_1);
	} else if (String_Equals(s, $("HTTP/1.0"))) {
		return ref(1_0);
	}

	return ref(Unset);
}
