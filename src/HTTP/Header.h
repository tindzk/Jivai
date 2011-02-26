#import "Query.h"
#import "Method.h"
#import "Version.h"

#import "../HTTP.h"
#import "../Char.h"
#import "../String.h"
#import "../Integer.h"
#import "../Exception.h"

#define self HTTP_Header

// @exc EmptyRequestUri
// @exc RequestMalformed
// @exc UnknownMethod
// @exc UnknownStatus
// @exc UnknownVersion

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

class {
	ref(Events) events;
};

def(void, Init, ref(Events) events);
def(void, ParseMethod, ProtString s);
def(void, ParseVersion, ProtString s);
def(void, ParseStatus, ProtString s);
def(void, ParseUri, ProtString s);
def(void, ParseHeaderLine, ProtString s);
sdef(ssize_t, GetLength, ProtString str);
def(void, Parse, ref(Type) type, ProtString s);

#undef self
