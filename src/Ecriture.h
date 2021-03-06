#import "String.h"
#import "Exception.h"

#define self Ecriture

set(ref(TokenType)) {
	ref(TokenType_Unset),
	ref(TokenType_TagStart),
	ref(TokenType_Option),    /* Requires unescaping. */
	ref(TokenType_AttrName),  /* Requires unescaping. */
	ref(TokenType_AttrValue), /* Requires unescaping. */
	ref(TokenType_Literal),   /* Requires unescaping. */
	ref(TokenType_Value),
	ref(TokenType_Comment),
	ref(TokenType_TagEnd),
	ref(TokenType_Done)
};

Callback(ref(OnToken),      void, ref(TokenType) type, RdString value, size_t line);
Callback(ref(OnBuildToken), void, ref(TokenType) type, RdString value);

rsdef(CarrierString, Unescape, RdString str);
rsdef(CarrierString, Escape, RdString str, ref(TokenType) type);

#undef self
