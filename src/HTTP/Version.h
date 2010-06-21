#ifndef HTTP_VERSION_H
#define HTTP_VERSION_H

#include <String.h>

typedef enum {
	HTTP_Version_1_0,
	HTTP_Version_1_1,
	HTTP_Version_Unset
} HTTP_Version;

HTTP_Version HTTP_Version_FromString(String s);

#endif
