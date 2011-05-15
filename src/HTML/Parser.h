#import "../HTML.h"
#import "../String.h"
#import "../StringReader.h"

#define self HTML_Parser

class {
	HTML_OnToken onToken;
	StringReader reader;
};

rsdef(self, New, HTML_OnToken onToken);
def(void, Destroy);
def(void, Process, RdString s);

#undef self
