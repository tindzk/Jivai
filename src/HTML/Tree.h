#import "Entities.h"
#import "Tokenizer.h"

#import "../Tree.h"
#import "../Block.h"
#import "../Array.h"
#import "../String.h"
#import "../Exception.h"

#define self HTML_Tree

// @exc IllegalNesting

record(ref(Attr)) {
	String name;
	String value;
};

Array(ref(Attr), ref(Attrs));

record(ref(Node)) {
	Tree_Define(ref(Node));

	enum {
		ref(NodeType_Value),
		ref(NodeType_Tag)
	} type;

	String value;

	ref(Attrs) *attrs;
};

class {
	Tree tree;
	ref(Node) *node;

	size_t depth;
};

rsdef(self, New);
def(void, Destroy);
def(void, DestroyNode, ref(Node) *node);
def(ref(Node) *, GetRoot);
def(void, ProcessToken, HTML_Tokenizer_TokenType type, RdString value);
sdef(ref(Attr) *, GetAttr, ref(Node) *node, RdString name);
sdef(ref(Node) *, GetNodeByNames, ref(Node) *node, ...);
sdef(ref(Node) *, GetNodeByIds, ref(Node) *node, ...);

#undef self
