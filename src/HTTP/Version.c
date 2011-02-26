#import "Version.h"

#define self HTTP_Version

sdef(ProtString, ToString, self version) {
	switch (version) {
		case ref(1_0):
			return $("HTTP/1.0");

		case ref(1_1):
			return $("HTTP/1.1");

		case ref(Unset):
			return $("");
	}

	return $("");
}

sdef(self, FromString, ProtString s) {
	if (String_Equals(s, $("HTTP/1.1"))) {
		return ref(1_1);
	} else if (String_Equals(s, $("HTTP/1.0"))) {
		return ref(1_0);
	}

	return ref(Unset);
}
