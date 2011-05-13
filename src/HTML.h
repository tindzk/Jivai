#import "String.h"

#define self HTML

set(ref(TokenType)) {
	ref(TokenType_Unset),

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
	ref(TokenType_Option),
	ref(TokenType_AttrEnd),
	ref(TokenType_Done)
};

Callback(ref(OnToken), void, ref(TokenType) type, RdString value);

sdef(void, Unescape, String *html);

#undef self
