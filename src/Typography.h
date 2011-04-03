#import "Tree.h"
#import "String.h"
#import "Stream.h"
#import "Exception.h"

#define self Typography

// @exc IllegalNesting

record(ref(Item)) {
	String name;
	String options;
};

record(ref(Text)) {
	String value;
};

record(ref(Node)) {
	Tree_Define(ref(Node));

	size_t line;

	enum {
		ref(NodeType_Item),
		ref(NodeType_Text)
	} type;

	char data[0];
};

#define Typography_Item(node) \
	((Typography_Item *) &(node)->data)

static alwaysInline sdef(RdString, Item_GetName, ref(Node) *node) {
	return Typography_Item(node)->name.rd;
}

static alwaysInline sdef(RdString, Item_GetOptions, ref(Node) *node) {
	return Typography_Item(node)->options.rd;
}

#undef Typography_Item

#define Typography_Text(node) \
	((Typography_Text *) &(node)->data)

static alwaysInline sdef(RdString, Text_GetValue, ref(Node) *node) {
	return Typography_Text(node)->value.rd;
}

#undef Typography_Text

class {
	Stream stream;

	size_t line;

	Tree tree;
	ref(Node) *node;
};

rsdef(self, New);
def(void, Destroy);
def(void, DestroyNode, Tree_Node *ptr);
def(ref(Node) *, GetRoot);
def(void, Parse, Stream stream);

#undef self
