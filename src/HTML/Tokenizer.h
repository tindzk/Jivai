#import "../HTML.h"
#import "../String.h"
#import "../Exception.h"

#define self HTML_Tokenizer

class {
	HTML_OnToken onToken;
	RdString buf;
	size_t ofs;
};

rsdef(self, New, HTML_OnToken onToken);
def(void, Destroy);
def(void, Process, RdString s);

#undef self
