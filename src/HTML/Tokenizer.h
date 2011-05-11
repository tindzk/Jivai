#import "../String.h"
#import "../Exception.h"

#define self HTML_Tokenizer

set(ref(TokenType)) {
	ref(TokenType_Value),
	ref(TokenType_TagStart),
	ref(TokenType_TagEnd),
	ref(TokenType_Comment),
	ref(TokenType_AttrName),
	ref(TokenType_AttrValue),
	ref(TokenType_Option)
};

Callback(ref(OnToken), void, ref(TokenType), RdString);

class {
	ref(OnToken) onToken;
	RdString buf;
	size_t ofs;
};

rsdef(self, New, ref(OnToken) onToken);
def(void, Destroy);
def(void, Process, RdString s);

#undef self
