#import "String.h"
#import "HTTP/Status.h"
#import "HTTP/Method.h"
#import "HTTP/Version.h"

typedef void (* HTTP_OnPath)(void *, String);
typedef void (* HTTP_OnStatus)(void *, HTTP_Status);
typedef void (* HTTP_OnMethod)(void *, HTTP_Method);
typedef void (* HTTP_OnVersion)(void *, HTTP_Version);
typedef void (* HTTP_OnHeader)(void *, String, String);
typedef String* (* HTTP_OnParameter)(void *, String);
