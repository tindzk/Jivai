#import "../Bit.h"
#import "../BitMask.h"
#import "../HTML.h"
#import "../String.h"
#import "../StreamInterface.h"

#undef self
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

set(ref(State)) {
	ref(State_Tag)       = Bit(0),
	ref(State_TagName)   = Bit(1),
	ref(State_AttrName)  = Bit(2),
	ref(State_AttrValue) = Bit(3),
	ref(State_Quote)     = Bit(4),
	ref(State_Comment)   = Bit(5)
};

DefineCallback(ref(OnToken), void, ref(TokenType), String);

class(self) {
	ref(OnToken) onToken;

	String buf;
	String curToken;

	char curQuote;
	char last;

	int state;
};

def(void, Init, ref(OnToken) onToken);
def(void, Destroy);
def(void, Reset);
def(void, ProcessChar, char c);
def(void, Poll);
overload def(void, Process, String s);
overload def(void, Process, StreamInterface *stream, void *context);
