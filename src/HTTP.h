#ifndef HTTP_H
#define HTTP_H

#include "String.h"
#include "HTTP/Status.h"
#include "HTTP/Method.h"
#include "HTTP/Version.h"

typedef void (* HTTP_OnPath)(void *, String);
typedef void (* HTTP_OnStatus)(void *, HTTP_Status);
typedef void (* HTTP_OnMethod)(void *, HTTP_Method);
typedef void (* HTTP_OnVersion)(void *, HTTP_Version);
typedef void (* HTTP_OnHeader)(void *, String, String);
typedef String* (* HTTP_OnParameter)(void *, String);

#endif
