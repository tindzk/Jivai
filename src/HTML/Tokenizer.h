#import "../String.h"
#import "../Exception.h"

#define self HTML_Tokenizer

set(ref(TokenType)) {
	/* Refers to DOCTYPE. */
	ref(TokenType_Type),

	/* For CDATA tags. This type should not be merged with TokenType_Value as
	 * CDATA values don't require unescaping. Normal values, however, are not
	 * guaranteed to be already escaped.
	 */
	ref(TokenType_Data),

	/* Must be unescaped via HTML_Unescape(). Even then it may still contain
	 * entities which HTML_Entities_Decode() will convert to their UTF-8
	 * equivalents.
	 */
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
