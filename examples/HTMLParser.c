#import <Main.h>
#import <Path.h>
#import <File.h>
#import <HTML/Parser.h>
#import <HTML/Quirks.h>

#define self Application

static size_t depth = 0;

def(void, OnToken, HTML_TokenType type, RdString value) {
	if (type == HTML_TokenType_AttrEnd || type == HTML_TokenType_Done) {
		return;
	}

	if (type == HTML_TokenType_TagEnd) {
		assert(depth > 0);
		depth--;
	}

	rpt (depth) {
		String_Print($("    "));
	}

	if (type == HTML_TokenType_TagStart) {
		depth++;
	}

	if (type == HTML_TokenType_Type) {
		String_Print($("type"));
	} else if (type == HTML_TokenType_Value) {
		String_Print($("value"));
	} else if (type == HTML_TokenType_Data) {
		String_Print($("data"));
	} else if (type == HTML_TokenType_TagStart) {
		String_Print($("tag start"));
	} else if (type == HTML_TokenType_TagEnd) {
		String_Print($("tag end"));
	} else if (type == HTML_TokenType_Comment) {
		String_Print($("comment"));
	} else if (type == HTML_TokenType_AttrName) {
		String_Print($("attr name"));
	} else if (type == HTML_TokenType_AttrValue) {
		String_Print($("attr value"));
	} else if (type == HTML_TokenType_Option) {
		String_Print($("option"));
	}

	String_Print($(": '"));
	String_Print(value);
	String_Print($("'\n"));
}

def(bool, Run) {
	RdString path =
		(this->args->len == 0)
			? $("HTMLParser.html")
			: this->args->buf[0];

	String s = String_New((size_t) Path_GetSize(path));
	File_GetContents(path, &s);

	HTML_Quirks quirks = HTML_Quirks_New(
		HTML_OnToken_For(this, ref(OnToken)));
	HTML_Parser html = HTML_Parser_New(
		HTML_OnToken_For(&quirks, HTML_Quirks_ProcessToken));

	HTML_Parser_Process(&html, s.rd);

	HTML_Parser_Destroy(&html);
	HTML_Quirks_Destroy(&quirks);

	String_Destroy(&s);

	return true;
}
