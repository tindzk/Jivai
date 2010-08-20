#import "../String.h"

typedef enum {
	HTTP_Method_Head,
	HTTP_Method_Get,
	HTTP_Method_Post,
	HTTP_Method_Unset
} HTTP_Method;

HTTP_Method HTTP_Method_FromString(String s);
String HTTP_Method_ToString(HTTP_Method method);
