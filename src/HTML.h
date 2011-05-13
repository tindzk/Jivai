#import "String.h"

#define self HTML

set(ref(TokenType)) {
	ref(TokenType_Unset),

	/* The parser doesn't ignore comments. */
	ref(TokenType_Comment),

	/* Refers to DOCTYPE. */
	ref(TokenType_Type),

	/* For CDATA tags. This type should not be merged with TokenType_Value as
	 * CDATA values don't require unescaping. Normal values, however, are not
	 * guaranteed to be already escaped.
	 */
	ref(TokenType_Data),

	/* Must be decoded using HTML_Entities_Decode() which will convert the
	 * entities to their UTF-8 equivalents.
	 */
	ref(TokenType_Value),

	/* Generated when a new tag begins. */
	ref(TokenType_TagStart),

	/* This is generated when a tag is closed. If the value is empty, it refers
	 * to the most recently added tag. This is the case for XHTML tags like
	 * `<br />' whereas for `<br></br>' the value is `br'.
	 */
	ref(TokenType_TagEnd),

	/* For attribute names. No decoding required. */
	ref(TokenType_AttrName),

	/* Must be unescaped via HTML_Unescape(). */
	ref(TokenType_AttrValue),

	/* An option is an attribute without value. */
	ref(TokenType_Option),

	/* Generated when parsing is done. */
	ref(TokenType_Done)
};

static inline rsdef(bool, IsTagAttr, ref(TokenType) type) {
	return type == ref(TokenType_AttrName)
		|| type == ref(TokenType_AttrValue)
		|| type == ref(TokenType_Option);
}

Callback(ref(OnToken), void, ref(TokenType) type, RdString value);

overload sdef(void, Unescape, String *str);
overload sdef(String, Unescape, RdString str);

#undef self
