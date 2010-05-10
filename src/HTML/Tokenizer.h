#ifndef HTML_TOKENIZER_H
#define HTML_TOKENIZER_H

#include "../Bit.h"
#include "../BitMask.h"
#include "../HTML.h"
#include "../String.h"
#include "../StreamInterface.h"

typedef enum {
	HTML_Tokenizer_TokenType_Value,
	HTML_Tokenizer_TokenType_TagStart,
	HTML_Tokenizer_TokenType_TagEnd,
	HTML_Tokenizer_TokenType_Comment,
	HTML_Tokenizer_TokenType_AttrName,
	HTML_Tokenizer_TokenType_AttrValue,
	HTML_Tokenizer_TokenType_Option
} HTML_Tokenizer_TokenType;

typedef enum {
	HTML_Tokenizer_State_Tag       = Bit(0),
	HTML_Tokenizer_State_TagName   = Bit(1),
	HTML_Tokenizer_State_AttrName  = Bit(2),
	HTML_Tokenizer_State_AttrValue = Bit(3),
	HTML_Tokenizer_State_Quote     = Bit(4),
	HTML_Tokenizer_State_Comment   = Bit(5)
} HTML_Tokenizer_State;

typedef void (*HTML_Tokenizer_OnToken)(void *, HTML_Tokenizer_TokenType, String);

typedef struct {
	HTML_Tokenizer_OnToken onToken;
	void *context;

	String buf;
	String curToken;

	char curQuote;
	char last;

	int state;
} HTML_Tokenizer;

void HTML_Tokenizer_Init(HTML_Tokenizer *this, HTML_Tokenizer_OnToken onToken, void *context);
void HTML_Tokenizer_Destroy(HTML_Tokenizer *this);
void HTML_Tokenizer_Reset(HTML_Tokenizer *this);
void HTML_Tokenizer_ProcessChar(HTML_Tokenizer *this, char c);
void HTML_Tokenizer_ProcessString(HTML_Tokenizer *this, String s);
void HTML_Tokenizer_ProcessStream(HTML_Tokenizer *this, StreamInterface *stream, void *context);
void HTML_Tokenizer_Poll(HTML_Tokenizer *this);

#endif
