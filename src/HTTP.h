#import "String.h"
#import "HTTP/Status.h"
#import "HTTP/Method.h"
#import "HTTP/Version.h"

#define self HTTP

record(ref(RequestInfo)) {
	ref(Version) version;
	ref(Method)  method;
	RdString     path;
};

record(ref(ResponseInfo)) {
	ref(Version) version;
	ref(Status)  status;
};

Callback(ref(OnStatus), void, ref(Status) status);
Callback(ref(OnRequestInfo), void, ref(RequestInfo) info);
Callback(ref(OnResponseInfo), void, ref(ResponseInfo) info);
Callback(ref(OnHeader), void, RdString name, RdString value);
Callback(ref(OnParameter), String *, RdString name);

#undef self
