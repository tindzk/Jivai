#import "String.h"
#import "HTTP/Status.h"
#import "HTTP/Method.h"
#import "HTTP/Version.h"

#undef self
#define self HTTP

DefineCallback(ref(OnPath), void, String);
DefineCallback(ref(OnStatus), void, ref(Status));
DefineCallback(ref(OnMethod), void, ref(Method));
DefineCallback(ref(OnVersion), void, ref(Version));
DefineCallback(ref(OnHeader), void, String, String);
DefineCallback(ref(OnParameter), String *, String);
