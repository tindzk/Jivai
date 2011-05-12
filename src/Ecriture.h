#import "Tree.h"
#import "String.h"
#import "Stream.h"
#import "Exception.h"

#define self Ecriture

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

#define Ecriture_Item(node) \
	((Ecriture_Item *) &(node)->data)

static alwaysInline sdef(RdString, Item_GetName, ref(Node) *node) {
	return Ecriture_Item(node)->name.rd;
}

static alwaysInline sdef(RdString, Item_GetOptions, ref(Node) *node) {
	return Ecriture_Item(node)->options.rd;
}

#undef Ecriture_Item

#define Ecriture_Text(node) \
	((Ecriture_Text *) &(node)->data)

static alwaysInline sdef(RdString, Text_GetValue, ref(Node) *node) {
	return Ecriture_Text(node)->value.rd;
}

#undef Ecriture_Text

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
