#import "../HTML.h"
#import "../Stream.h"
#import "../String.h"
#import "../Exception.h"

#define self HTML_Builder

class {
	Stream stream;
	bool mustClose;
};

rsdef(self, New, Stream stream);
def(void, Destroy);
def(void, ProcessToken, HTML_TokenType type, RdString value);

#undef self
