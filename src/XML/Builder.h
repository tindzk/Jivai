#import "../XML.h"
#import "../Stream.h"
#import "../String.h"
#import "../Exception.h"

#define self XML_Builder

class {
	Stream stream;
	bool mustClose;
};

rsdef(self, New, Stream stream);
def(void, Destroy);
def(void, ProcessToken, XML_TokenType type, RdString value);

#undef self
