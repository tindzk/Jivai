#include "Method.h"

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
