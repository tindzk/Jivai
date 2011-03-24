#import "String.h"
#import "HTTP/Status.h"
#import "HTTP/Method.h"
#import "HTTP/Version.h"

#define self HTTP

Callback(ref(OnPath), void, RdString);
Callback(ref(OnStatus), void, ref(Status));
Callback(ref(OnMethod), void, ref(Method));
Callback(ref(OnVersion), void, ref(Version));
Callback(ref(OnHeader), void, RdString, RdString);
Callback(ref(OnParameter), String *, RdString);

#undef self
