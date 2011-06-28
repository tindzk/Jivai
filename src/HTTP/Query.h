#import "../Hex.h"
#import "../HTTP.h"
#import "../String.h"
#import "../Exception.h"

#define self HTTP_Query

exc(ExceedsPermittedLength)

class {
	HTTP_OnParameter onParameter;
	bool autoResize;
};

def(void, Init, HTTP_OnParameter onParameter);
def(void, SetAutoResize, bool value);
sdef(size_t, GetAbsoluteLength, RdString s);
sdef(void, Unescape, RdString src, char *dst, bool isFormUri);
sdef(String, Encode, RdString param);
def(void, Decode, RdString s, bool isFormUri);

#undef self
