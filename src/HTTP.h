#import "String.h"
#import "HTTP/Status.h"
#import "HTTP/Method.h"
#import "HTTP/Version.h"

#define self HTTP

DefineCallback(ref(OnPath), void, ProtString);
DefineCallback(ref(OnStatus), void, ref(Status));
DefineCallback(ref(OnMethod), void, ref(Method));
DefineCallback(ref(OnVersion), void, ref(Version));
DefineCallback(ref(OnHeader), void, ProtString, ProtString);
DefineCallback(ref(OnParameter), String *, ProtString);

#undef self
