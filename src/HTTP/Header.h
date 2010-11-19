#import "Query.h"
#import "Method.h"
#import "Version.h"

#import "../HTTP.h"
#import "../Char.h"
#import "../String.h"
#import "../Integer.h"
#import "../Exception.h"

#undef self
#define self HTTP_Header

enum {
	excEmptyRequestUri = excOffset,
	excRequestMalformed,
	excUnknownMethod,
	excUnknownStatus,
	excUnknownVersion
};

set(ref(Type)) {
	ref(Type_Request),
	ref(Type_Response)
};

record(ref(Events)) {
	HTTP_OnParameter onParameter;
	HTTP_OnVersion   onVersion;
	HTTP_OnMethod    onMethod;
	HTTP_OnHeader    onHeader;
	HTTP_OnStatus    onStatus;
	HTTP_OnPath      onPath;
};

class(self) {
	ref(Events) events;
};

void HTTP_Header0(ExceptionManager *e);

def(void, Init, ref(Events) events);
def(void, ParseMethod, String s);
def(void, ParseVersion, String s);
def(void, ParseStatus, String s);
def(void, ParseUri, String s);
def(void, ParseHeaderLine, String s);
sdef(ssize_t, GetLength, String str);
def(void, Parse, ref(Type) type, String s);
