#import "../XML.h"
#import "../String.h"
#import "../StringReader.h"

#define self XML_Parser

class {
	XML_OnToken onToken;
	StringReader reader;
};

rsdef(self, New, XML_OnToken onToken);
def(void, Destroy);
def(void, Process, RdString s);

#undef self
