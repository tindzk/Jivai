#import <Main.h>
#import <Path.h>
#import <File.h>
#import <HTML/Parser.h>

#define self Application

static size_t depth = 0;

def(void, OnToken, XML_TokenType type, RdString value) {
	if (type == XML_TokenType_Done) {
		return;
	}

	if (type == XML_TokenType_TagEnd) {
		assert(depth > 0);
		depth--;
	}

	rpt (depth) {
		String_Print($("    "));
	}

	if (type == XML_TokenType_TagStart) {
		depth++;
	}

	if (type == XML_TokenType_Type) {
		String_Print($("type"));
	} else if (type == XML_TokenType_Value) {
		String_Print($("value"));
	} else if (type == XML_TokenType_Data) {
		String_Print($("data"));
	} else if (type == XML_TokenType_TagStart) {
		String_Print($("tag start"));
	} else if (type == XML_TokenType_TagEnd) {
		String_Print($("tag end"));
	} else if (type == XML_TokenType_Comment) {
		String_Print($("comment"));
	} else if (type == XML_TokenType_AttrName) {
		String_Print($("attr name"));
	} else if (type == XML_TokenType_AttrValue) {
		String_Print($("attr value"));
	} else if (type == XML_TokenType_Option) {
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

	HTML_Parser html = HTML_Parser_New(XML_OnToken_For(this, ref(OnToken)));
	HTML_Parser_Initialize(&html);
	HTML_Parser_Process(&html, s.rd);
	HTML_Parser_Destroy(&html);

	String_Destroy(&s);

	return true;
}
