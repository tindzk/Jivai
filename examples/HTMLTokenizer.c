#import <Main.h>
#import <Terminal.h>
#import <FileStream.h>
#import <BufferedStream.h>
#import <HTML/Tokenizer.h>

#define self Application

static size_t depth = 0;

def(void, OnToken, HTML_Tokenizer_TokenType type, RdString value) {
	if (type == HTML_Tokenizer_TokenType_TagEnd) {
		assert(depth > 0);
		depth--;
	}

	rpt (depth) {
		String_Print($("    "));
	}

	if (type == HTML_Tokenizer_TokenType_TagStart) {
		depth++;
	}

	if (type == HTML_Tokenizer_TokenType_Value) {
		String_Print($("value"));
	} else if (type == HTML_Tokenizer_TokenType_TagStart) {
		String_Print($("tag start"));
	} else if (type == HTML_Tokenizer_TokenType_TagEnd) {
		String_Print($("tag end"));
	} else if (type == HTML_Tokenizer_TokenType_Comment) {
		String_Print($("comment"));
	} else if (type == HTML_Tokenizer_TokenType_AttrName) {
		String_Print($("attr name"));
	} else if (type == HTML_Tokenizer_TokenType_AttrValue) {
		String_Print($("attr value"));
	} else if (type == HTML_Tokenizer_TokenType_Option) {
		String_Print($("option"));
	}

	String_Print($(": "));
	String_Print(value);
	String_Print($("\n"));
}

def(bool, Run) {
	RdString name =
		(this->args->len == 0)
			? $("HTMLTokenizer.html")
			: this->args->buf[0];

	File file;

	try {
		file = File_New(name, FileStatus_ReadOnly);
	} catch (File, NotFound) {
		String_Print($("File not found.\n"));
		excReturn false;
	} finally {

	} tryEnd;

	BufferedStream stream = BufferedStream_New(File_AsStream(&file));
	BufferedStream_SetInputBuffer(&stream, 1024, 128);

	HTML_Tokenizer html = HTML_Tokenizer_New(
		HTML_Tokenizer_OnToken_For(this, ref(OnToken)));
	HTML_Tokenizer_Process(&html, BufferedStream_AsStream(&stream));
	HTML_Tokenizer_Destroy(&html);

	BufferedStream_Close(&stream);
	BufferedStream_Destroy(&stream);

	return true;
}
