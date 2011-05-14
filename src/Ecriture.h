#import "String.h"

#define self Ecriture

set(ref(TokenType)) {
	ref(TokenType_Unset),
	ref(TokenType_TagStart),
	ref(TokenType_Option),
	ref(TokenType_Value),
	ref(TokenType_TagEnd),
	ref(TokenType_Done)
};

Callback(ref(OnToken), void, ref(TokenType) type, String value, size_t line);

#undef self
