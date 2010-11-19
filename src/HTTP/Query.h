#import "../Hex.h"
#import "../HTTP.h"
#import "../String.h"
#import "../Exception.h"

#undef self
#define self HTTP_Query

enum {
	excExceedsPermittedLength = excOffset
};

class(self) {
	HTTP_OnParameter onParameter;
	bool autoResize;
};

void HTTP_Query0(ExceptionManager *e);

def(void, Init, HTTP_OnParameter onParameter);
def(void, SetAutoResize, bool value);
sdef(size_t, GetAbsoluteLength, String s);
sdef(void, Unescape, String src, char *dst, bool isFormUri);
def(void, Decode, String s, bool isFormUri);
