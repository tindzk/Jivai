#import "../HTML.h"

#define self HTML_Quirks

record(ref(Element)) {
	HTML_TokenType type;
	RdString value;
};

class {
	bool toClose;
	RdString tagName;
	HTML_OnToken onToken;
};

rsdef(self, New, HTML_OnToken onToken);
def(void, Destroy);
def(void, ProcessToken, HTML_TokenType type, RdString value);

#undef self
