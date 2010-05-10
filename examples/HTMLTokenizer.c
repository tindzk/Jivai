#include <FileStream.h>
#include <BufferedStream.h>
#include <HTML/Tokenizer.h>

ExceptionManager exc;

static size_t depth = 0;

void OnToken(UNUSED void *p, HTML_Tokenizer_TokenType type, String value) {
	if (type == HTML_Tokenizer_TokenType_TagEnd) {
		depth--;
	}

	size_t i = depth;

	while (i--) {
		String_Print(String("    "));
	}

	if (type == HTML_Tokenizer_TokenType_TagStart) {
		depth++;
	}

	if (type == HTML_Tokenizer_TokenType_Value) {
		String_Print(String("value"));
	} else if (type == HTML_Tokenizer_TokenType_TagStart) {
		String_Print(String("tag start"));
	} else if (type == HTML_Tokenizer_TokenType_TagEnd) {
		String_Print(String("tag end"));
	} else if (type == HTML_Tokenizer_TokenType_Comment) {
		String_Print(String("comment"));
	} else if (type == HTML_Tokenizer_TokenType_AttrName) {
		String_Print(String("attr name"));
	} else if (type == HTML_Tokenizer_TokenType_AttrValue) {
		String_Print(String("attr value"));
	} else if (type == HTML_Tokenizer_TokenType_Option) {
		String_Print(String("option"));
	}

	String_Print(String(": "));
	String_Print(value);
	String_Print(String("\n"));
}

int main(void) {
	ExceptionManager_Init(&exc);

	File0(&exc);
	String0(&exc);
	Memory0(&exc);

	File file;
	BufferedStream stream;

	try (&exc) {
		FileStream_Open(&file, String("HTMLTokenizer.html"), O_RDONLY);
	} catch(&File_NotFoundException, e) {
		String_Print(String("File not found.\n"));
		return EXIT_FAILURE;
	} finally {

	} tryEnd;

	BufferedStream_Init(&stream, &FileStream_Methods, &file);
	BufferedStream_SetInputBuffer(&stream, 1024, 128);

	HTML_Tokenizer html;
	HTML_Tokenizer_Init(&html, &OnToken, NULL);
	HTML_Tokenizer_ProcessStream(&html, &BufferedStream_Methods, &stream);
	HTML_Tokenizer_Destroy(&html);

	BufferedStream_Close(&stream);
	BufferedStream_Destroy(&stream);

	return EXIT_SUCCESS;
}
