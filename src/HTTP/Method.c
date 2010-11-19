#import "Method.h"

sdef(self, FromString, String s) {
	if (String_Equals(s, $("GET"))) {
		return ref(Get);
	} else if (String_Equals(s, $("POST"))) {
		return ref(Post);
	} else if (String_Equals(s, $("HEAD"))) {
		return ref(Head);
	}

	return ref(Unset);
}

sdef(String, ToString, self method) {
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
