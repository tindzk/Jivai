#import "Tokenizer.h"

#define self HTML_Quirks

record(ref(Element)) {
	HTML_Tokenizer_TokenType type;
	RdString value;
};

class {
	ref(Element) prev;
	HTML_Tokenizer_OnToken onToken;
};

rsdef(self, New, HTML_Tokenizer_OnToken onToken);
def(void, Destroy);
def(void, ProcessToken, HTML_Tokenizer_TokenType type, RdString value);

#undef self
