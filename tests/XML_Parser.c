#import <XML/Parser.h>

#import "TestSuite.h"

#define self tsXML_Parser

record(ref(Element)) {
	XML_TokenType type;
	RdString value;
};

class {
	XML_Parser xml;
	ref(Element) elements[64];
	size_t cur;
	size_t count;
};

tsRegister("XML.Parser") {
	return true;
}

def(void, OnToken, XML_TokenType type, RdString value);

tsInit {
	this->xml = XML_Parser_New(XML_OnToken_For(this, ref(OnToken)));

	fwd (i, nElems(this->elements)) {
		this->elements[i] = (ref(Element)) { .value = $("") };
	}

	this->cur   = 0;
	this->count = 0;
}

tsDestroy {
	XML_Parser_Destroy(&this->xml);
}

def(void, OnToken, XML_TokenType type, RdString value) {
	this->elements[this->count] = (ref(Element)) {
		.type  = type,
		.value = value
	};

	this->count++;
}

def(bool, Matches, XML_TokenType type, RdString value) {
	if (this->cur >= this->count) {
		return false;
	}

	this->cur++;

	if (type != this->elements[this->cur - 1].type) {
		return false;
	}

	if (!String_Equals(value, this->elements[this->cur - 1].value)) {
		return false;
	}

	return true;
}

def(void, Process, RdString str) {
	this->cur   = 0;
	this->count = 0;

	XML_Parser_Process(&this->xml, str);
}

tsCase(Acute, "Value") {
	call(Process, $("  Hello World!  "));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Value, $("  Hello World!  ")));

	call(Process, $("  "));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Value, $("  ")));
}

tsCase(Acute, "Comments") {
	call(Process, $("<!---->"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Comment, $("")));

	call(Process, $("<!--Comment-->"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Comment, $("Comment")));

	call(Process, $("<!-- Comment -->"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Comment, $(" Comment ")));

	call(Process, $("<!--[if IE]><link rel=\"stylesheet\" /><![endif]-->"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Comment,
			$("[if IE]><link rel=\"stylesheet\" /><![endif]")));
}

tsCase(Acute, "Tags (simple)") {
	call(Process, $("<br></br>"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("br")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagEnd, $("br")));
}

tsCase(Acute, "Tags (auto-closing)") {
	call(Process, $("<br/>"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("br")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagEnd, $("")));

	call(Process, $("<br option/>"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("br")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Option, $("option")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagEnd, $("")));

	call(Process, $("<br />"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("br")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagEnd, $("")));

	call(Process, $("<br option    />"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("br")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Option, $("option")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagEnd, $("")));
}

tsCase(Acute, "Tags (malformed)") {
	call(Process, $("<input type = text />"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("input")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_AttrName, $("type")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_AttrValue, $("text")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagEnd, $("")));

	call(Process, $("<input  option  type = text  option />"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("input")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Option, $("option")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_AttrName, $("type")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_AttrValue, $("text")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Option, $("option")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagEnd, $("")));

	call(Process, $("<input   type=text   value=val/></input>"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("input")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_AttrName, $("type")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_AttrValue, $("text")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_AttrName, $("value")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_AttrValue, $("val/")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagEnd, $("input")));

	call(Process, $("<input type/>"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("input")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Option, $("type")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagEnd, $("")));

	call(Process, $("<input type/ >"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("input")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Option, $("type/")));
}

tsCase(Acute, "Tags (single quotes)") {
	call(Process, $("<img alt='' src='http://localhost/' />"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("img")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_AttrName, $("alt")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_AttrValue, $("''")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_AttrName, $("src")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_AttrValue, $("'http://localhost/'")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagEnd, $("")));
}

tsCase(Acute, "Tags (double quotes)") {
	call(Process, $("<a href=\"http://localhost/\">Caption</a>"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("a")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_AttrName, $("href")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_AttrValue, $("\"http://localhost/\"")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Value, $("Caption")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagEnd, $("a")));
}

tsCase(Acute, "Tags (escaping quotes)") {
	call(Process, $("<img alt=\"This needs to be \\\"escaped\\\".\" \">"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("img")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_AttrName, $("alt")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_AttrValue,
			$("\"This needs to be \\\"escaped\\\".\"")));
}

tsCase(Acute, "JavaScript") {
	call(Process, $(
		"<script type=\"text/javascript\">\n"
			"function getCookieVal (offset) {\n"
				"\tvar endstr = document.cookie.indexOf(\";\", offset);\n"
				"\tif (endstr == -1)\n"
					"\t\tendstr = document.cookie.length;\n"
				"\treturn unescape(document.cookie.substring(offset, endstr));\n"
			"}\n"
		"</script>"
	));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagStart, $("script")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_AttrName, $("type")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_AttrValue, $("\"text/javascript\"")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Value, $(
			"\nfunction getCookieVal (offset) {\n"
					"\tvar endstr = document.cookie.indexOf(\";\", offset);\n"
					"\tif (endstr == -1)\n"
							"\t\tendstr = document.cookie.length;\n"
					"\treturn unescape(document.cookie.substring(offset, endstr));\n"
			"}\n"
	)));

	Assert($("Matches"),
		call(Matches, XML_TokenType_TagEnd, $("script")));
}

tsCase(Acute, "CDATA") {
	call(Process, $("<![CDATA[<sender>John Smith</sender>]]>"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Data,
			$("<sender>John Smith</sender>")));

	/* CDATA cannot contain the ]]> sequence. The tokenizer won't merge the
	 * contents of two blocks.
	 */
	call(Process, $("<![CDATA[Start]]]]><![CDATA[>End]]>"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Data, $("Start]]")));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Data, $(">End")));
}

tsCase(Acute, "DOCTYPE") {
	call(Process,
		$("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" "
			"\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">"));

	Assert($("Matches"),
		call(Matches, XML_TokenType_Type,
			$("html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" "
				"\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\"")));
}

tsCase(Acute, "Invalid tags") {
	/* Incomplete tags without attributes. */
	call(Process, $("<"));
	Assert($("Matches"), call(Matches, XML_TokenType_Value, $("<")));

	call(Process, $("< "));
	Assert($("Matches"), call(Matches, XML_TokenType_Value, $("< ")));

	/* UTF-8 tags should be ignored. */
	call(Process, $("<œ"));
	Assert($("Matches"), call(Matches, XML_TokenType_Value, $("<œ")));
	call(Process, $("<œ option"));
	Assert($("Matches"), call(Matches, XML_TokenType_Value, $("<œ option")));
	call(Process, $("<œ option>"));
	Assert($("Matches"), call(Matches, XML_TokenType_Value, $("<œ option>")));

	/* Valid but incomplete tags must not yield tokens. */
	call(Process, $("<name"));
	Assert($("Matches"), !call(Matches, XML_TokenType_Value, $("<name")));

	call(Process, $("a <= b || c >= d || e < f || g > h"));
	Assert($("Matches"),
		call(Matches, XML_TokenType_Value,
			$("a <= b || c >= d || e < f || g > h")));
}

tsCase(Acute, "Nesting") {
	call(Process, $("value<p><b>parag</b></p>"));
	Assert($("Matches"), call(Matches, XML_TokenType_Value, $("value")));
	Assert($("Matches"), call(Matches, XML_TokenType_TagStart, $("p")));
	Assert($("Matches"), call(Matches, XML_TokenType_TagStart, $("b")));
	Assert($("Matches"), call(Matches, XML_TokenType_Value, $("parag")));
	Assert($("Matches"), call(Matches, XML_TokenType_TagEnd, $("b")));
	Assert($("Matches"), call(Matches, XML_TokenType_TagEnd, $("p")));
}
