#import "../HTML.h"
#import "../XML/Parser.h"

#define self HTML_Parser

class {
	bool toClose;
	RdString tagName;
	XML_Parser parser;
	XML_OnToken onToken;
};

rsdef(self, New, XML_OnToken onToken);
def(void, Destroy);
def(void, Initialize);
def(void, Process, RdString s);

#undef self
