#import "String.h"

#define self Ecriture

set(ref(TokenType)) {
	ref(TokenType_Unset),
	ref(TokenType_TagStart),
	ref(TokenType_Option), /* Requires unescaping. */
	ref(TokenType_Value),
	ref(TokenType_Literal), /* Requires unescaping. */
	ref(TokenType_TagEnd),
	ref(TokenType_Done)
};

Callback(ref(OnToken), void, ref(TokenType) type, RdString value, size_t line);

rsdef(CarrierString, Unescape, RdString str);

#undef self
