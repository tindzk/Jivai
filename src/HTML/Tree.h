#import "Entities.h"
#import "Tokenizer.h"

#import "../Tree.h"
#import "../Block.h"
#import "../Array.h"
#import "../String.h"
#import "../Exception.h"

#undef self
#define self HTML_Tree

record(ref(Attr)) {
	String name;
	String value;
};

record(ref(Node)) {
	Tree_Define(ref(Node));

	enum {
		ref(NodeType_Value),
		ref(NodeType_Tag)
	} type;

	String value;

	Array(ref(Attr), *attrs);
};

class(self) {
	Tree tree;
	ref(Node) *node;

	size_t depth;
};

void HTML_Tree0(ExceptionManager *e);

def(void, Init);
def(void, Destroy);
sdef(void, DestroyNode, ref(Node) *node);
def(ref(Node) *, GetRoot);
def(void, ProcessToken, HTML_Tokenizer_TokenType type, String value);
sdef(ref(Attr) *, GetAttr, ref(Node) *node, String name);
sdef(ref(Node) *, GetNodeByNames, ref(Node) *node, ...);
sdef(ref(Node) *, GetNodeByIds, ref(Node) *node, ...);
sdef(void, Foreach, ref(Node) *node, void (^cb)(ref(Node) *));
