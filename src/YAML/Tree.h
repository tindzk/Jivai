#import "../Tree.h"
#import "../YAML.h"
#import "../String.h"
#import "../Exception.h"

#define self YAML_Tree

record(ref(Node)) {
	Tree_Define(ref(Node));

	enum {
		ref(NodeType_Node),
		ref(NodeType_Section),
		ref(NodeType_Item)
	} type;

	RdString name;

	/* Only used for NodeType_Item. */
	RdString value;
};

class {
	Tree tree;
	ref(Node) *node;
};

rsdef(self, New);
def(void, Destroy);
def(ref(Node) *, GetRoot);
def(void, Initialize);
def(void, ProcessToken, YAML_TokenType type, RdString value);

#undef self
