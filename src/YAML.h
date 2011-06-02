#import "String.h"

#define self YAML

set(ref(TokenType)) {
	ref(TokenType_Unset),
	ref(TokenType_Comment),
	ref(TokenType_Name),
	ref(TokenType_Value),
	ref(TokenType_Enter),
	ref(TokenType_Leave)
};

Callback(ref(OnToken), void, ref(TokenType) type, RdString value);

#undef self
