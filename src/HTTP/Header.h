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
// @exc ResponseMalformed
// @exc UnknownMethod
// @exc UnknownStatus
// @exc UnknownVersion

record(ref(Events)) {
	HTTP_OnRequestInfo  onRequestInfo;
	HTTP_OnResponseInfo onResponseInfo;
	HTTP_OnParameter    onParameter;
	HTTP_OnHeader       onHeader;
};

class {
	ref(Events) events;
	ssize_t pos1stLine;
};

rsdef(self, New, ref(Events) events);
def(HTTP_Method, ParseMethod, RdString s);
def(HTTP_Version, ParseVersion, RdString s);
def(HTTP_Status, ParseStatus, RdString s);
def(CarrierString, ParseUri, RdString s);
def(void, ParseUriParameters, RdString s);
sdef(ssize_t, GetLength, RdString str);
def(void, ParseRequest, RdString s);
def(void, ParseResponse, RdString s);

#undef self
