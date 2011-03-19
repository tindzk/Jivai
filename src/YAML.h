#import "Tree.h"
#import "String.h"
#import "Exception.h"
#import "StreamInterface.h"

#define self YAML

// @exc IllegalNesting

set(ref(NodeType)) {
	ref(NodeType_Node),
	ref(NodeType_Section),
	ref(NodeType_Item)
};

record(ref(Section)) {
	String name;
};

record(ref(Item)) {
	String key;
	String value;
};

record(ref(Node)) {
	Tree_Define(ref(Node));

	ref(NodeType) type;
	char data[0];
};

class {
	StreamInterface *stream;
	void *context;

	size_t line;

	Tree tree;
	ref(Node) *node;

	size_t depth;
	size_t depthWidth;
};

#define YAML_Section(node) \
	((YAML_Section *) &(node)->data)

static inline sdef(RdString, Section_GetName, YAML_Node *node) {
	return YAML_Section(node)->name.rd;
}

#undef YAML_Section

#define YAML_Item(node) \
	((YAML_Item *) &(node)->data)

static inline sdef(RdString, Item_GetKey, YAML_Node *node) {
	return YAML_Item(node)->key.rd;
}

static inline sdef(RdString, Item_GetValue, YAML_Node *node) {
	return YAML_Item(node)->value.rd;
}

#undef YAML_Item

rsdef(self, New, size_t depthWidth, StreamInterface *stream, void *context);
def(void, Destroy);
def(void, DestroyNode, ref(Node) *node);
def(ref(Node) *, GetRoot);
def(size_t, GetLine);
def(void *, Store, size_t depth, ref(NodeType) type, size_t size);
def(void, Parse);

#undef self
