#import "Method.h"

HTTP_Method HTTP_Method_FromString(String s) {
	if (String_Equals(s, String("GET"))) {
		return HTTP_Method_Get;
	} else if (String_Equals(s, String("POST"))) {
		return HTTP_Method_Post;
	} else if (String_Equals(s, String("HEAD"))) {
		return HTTP_Method_Head;
	}

	return HTTP_Method_Unset;
}

String HTTP_Method_ToString(HTTP_Method method) {
	switch (method) {
		case HTTP_Method_Head:
			return String("HEAD");

		case HTTP_Method_Get:
			return String("GET");

		case HTTP_Method_Post:
			return String("POST");

		default:
			return String("");
	}
}
