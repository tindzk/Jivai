#import <Terminal.h>
#import <FileStream.h>
#import <BufferedStream.h>
#import <HTML/Tokenizer.h>

static size_t depth = 0;

void OnToken(__unused void *p, HTML_Tokenizer_TokenType type, String value) {
	if (type == HTML_Tokenizer_TokenType_TagEnd) {
		depth--;
	}

	size_t i = depth;

	while (i--) {
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

int main(void) {
	File file;
	BufferedStream stream;

	try {
		FileStream_Open(&file, $("HTMLTokenizer.html"), FileStatus_ReadOnly);
	} clean catch (File, NotFound) {
		String_Print($("File not found.\n"));
		return ExitStatus_Failure;
	} finally {

	} tryEnd;

	BufferedStream_Init(&stream, File_AsStream(&file));
	BufferedStream_SetInputBuffer(&stream, 1024, 128);

	HTML_Tokenizer html;
	HTML_Tokenizer_Init(&html, Callback(NULL, &OnToken));
	HTML_Tokenizer_Process(HTML_Tokenizer_FromObject(&html),
		&BufferedStreamImpl, &stream);
	HTML_Tokenizer_Destroy(&html);

	BufferedStream_Close(&stream);
	BufferedStream_Destroy(&stream);

	return ExitStatus_Success;
}
