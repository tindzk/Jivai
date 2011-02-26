#import "Method.h"

#define self HTTP_Method

sdef(self, FromString, ProtString s) {
	if (String_Equals(s, $("GET"))) {
		return ref(Get);
	} else if (String_Equals(s, $("POST"))) {
		return ref(Post);
	} else if (String_Equals(s, $("HEAD"))) {
		return ref(Head);
	}

	return ref(Unset);
}

sdef(ProtString, ToString, self method) {
	switch (method) {
		case ref(Head):
			return $("HEAD");

		case ref(Get):
			return $("GET");

		case ref(Post):
			return $("POST");

		default:
			return $("");
	}
}
