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

static inline sdef(ProtString, Section_GetName, YAML_Node *node) {
	return YAML_Section(node)->name.prot;
}

#define YAML_Item(node) \
	((YAML_Item *) &(node)->data)

static inline sdef(ProtString, Item_GetKey, YAML_Node *node) {
	return YAML_Item(node)->key.prot;
}

static inline sdef(ProtString, Item_GetValue, YAML_Node *node) {
	return YAML_Item(node)->value.prot;
}

rsdef(self, New, size_t depthWidth, StreamInterface *stream, void *context);
def(void, Destroy);
void ref(DestroyNode)(ref(Node) *node);
def(ref(Node) *, GetRoot);
def(size_t, GetLine);
def(void *, Store, size_t depth, ref(NodeType) type, size_t size);
def(void, Parse);

#undef self
