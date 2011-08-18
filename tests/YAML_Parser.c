#import <YAML/Parser.h>

#import "TestSuite.h"

#define self tsYAML_Parser

record(ref(Element)) {
	YAML_TokenType type;
	RdString value;
};

class {
	YAML_Parser yml;
	ref(Element) elements[64];
	size_t cur;
	size_t count;
};

tsRegister("YAML.Parser") {
	return true;
}

def(void, OnToken, YAML_TokenType type, RdString value);

tsInit {
	this->yml = YAML_Parser_New(YAML_OnToken_For(this, ref(OnToken)));

	fwd (i, nElems(this->elements)) {
		this->elements[i] = (ref(Element)) { .value = $("") };
	}

	this->cur   = 0;
	this->count = 0;
}

tsDestroy {
	YAML_Parser_Destroy(&this->yml);
}

def(void, OnToken, YAML_TokenType type, RdString value) {
	this->elements[this->count] = (ref(Element)) {
		.type  = type,
		.value = value
	};

	this->count++;
}

def(bool, Matches, YAML_TokenType type, RdString value) {
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

	YAML_Parser_Process(&this->yml, str);
}

tsCase(Acute, "Comments") {
	call(Process, $("#Hello"));
	Assert($("Matches"), call(Matches, YAML_TokenType_Comment, $("Hello")));

	call(Process, $("# Hello "));
	Assert($("Matches"), call(Matches, YAML_TokenType_Comment, $(" Hello ")));

	call(Process, $("\t\t# Hello "));
	Assert($("Matches"), call(Matches, YAML_TokenType_Comment, $(" Hello ")));

	call(Process, $("\n# comment\n\n"));
	Assert($("Matches"), call(Matches, YAML_TokenType_Comment, $(" comment")));
}

tsCase(Acute, "Values") {
	call(Process, $("name:value"));
	Assert($("Matches"), call(Matches, YAML_TokenType_Name,  $("name")));
	Assert($("Matches"), call(Matches, YAML_TokenType_Value, $("value")));

	call(Process, $("name: value"));
	Assert($("Matches"), call(Matches, YAML_TokenType_Name,  $("name")));
	Assert($("Matches"), call(Matches, YAML_TokenType_Value, $("value")));

	call(Process, $("name : value"));
	Assert($("Matches"), call(Matches, YAML_TokenType_Name,  $("name")));
	Assert($("Matches"), call(Matches, YAML_TokenType_Value, $("value")));

	call(Process, $("name: value\n\n"));
	Assert($("Matches"), call(Matches, YAML_TokenType_Name,  $("name")));
	Assert($("Matches"), call(Matches, YAML_TokenType_Value, $("value")));
}

tsCase(Acute, "Nesting") {
	call(Process, $("section:\n\tname: value"));
	Assert($("Matches"), call(Matches, YAML_TokenType_Name,  $("section")));
	Assert($("Matches"), call(Matches, YAML_TokenType_Enter, $("")));
	Assert($("Matches"), call(Matches, YAML_TokenType_Name,  $("name")));
	Assert($("Matches"), call(Matches, YAML_TokenType_Value, $("value")));
	Assert($("Matches"), call(Matches, YAML_TokenType_Leave, $("")));
}

tsCase(Acute, "Exceptions") {
	bool caught = false;

	try {
		call(Process, $("section"));
	} catch(YAML_Parser, ColonMissing) {
		caught = true;
	} finally {

	} tryEnd;

	Assert($("Missing colon"), caught);

	try {
		caught = false;
		call(Process, $("section\n"));
	} catch(YAML_Parser, ColonMissing) {
		caught = true;
	} finally {

	} tryEnd;

	Assert($("Missing colon"), caught);

	/* Ignore indention when dealing with comments. */
	try {
		caught = false;
		call(Process, $("\t#Hello"));
	} catch(YAML_Parser, InvalidIndention) {
		caught = true;
	} finally {

	} tryEnd;

	Assert($("Invalid indention"), !caught);

	/* No indention expected (only after sections). */
	try {
		caught = false;
		call(Process, $("\tname: value"));
	} catch(YAML_Parser, InvalidIndention) {
		caught = true;
	} finally {

	} tryEnd;

	Assert($("Invalid indention"), caught);

	/* Indention expected. */
	try {
		caught = false;
		call(Process, $("name:\nname: value"));
	} catch(YAML_Parser, InvalidIndention) {
		caught = true;
	} finally {

	} tryEnd;

	Assert($("Invalid indention"), caught);
}

tsCase(Acute, "Complex") {
	call(Process, $(
		"a:\n"
			"\tb:\n"
			"\t\tc: d\n"
			"\te: f"));

	Assert($("Matches"), call(Matches, YAML_TokenType_Name,  $("a")));
	Assert($("Matches"), call(Matches, YAML_TokenType_Enter, $("")));
	Assert($("Matches"), call(Matches, YAML_TokenType_Name,  $("b")));
	Assert($("Matches"), call(Matches, YAML_TokenType_Enter, $("")));
	Assert($("Matches"), call(Matches, YAML_TokenType_Name,  $("c")));
	Assert($("Matches"), call(Matches, YAML_TokenType_Value, $("d")));
	Assert($("Matches"), call(Matches, YAML_TokenType_Leave, $("")));
	Assert($("Matches"), call(Matches, YAML_TokenType_Name,  $("e")));
	Assert($("Matches"), call(Matches, YAML_TokenType_Value, $("f")));
	Assert($("Matches"), call(Matches, YAML_TokenType_Leave, $("")));
}
