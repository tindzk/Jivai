#import "../Hex.h"
#import "../HTTP.h"
#import "../String.h"
#import "../Exception.h"

#define self HTTP_Query

// @exc ExceedsPermittedLength

class {
	HTTP_OnParameter onParameter;
	bool autoResize;
};

def(void, Init, HTTP_OnParameter onParameter);
def(void, SetAutoResize, bool value);
sdef(size_t, GetAbsoluteLength, ProtString s);
sdef(void, Unescape, ProtString src, char *dst, bool isFormUri);
sdef(String, Encode, ProtString param);
def(void, Decode, ProtString s, bool isFormUri);

#undef self
