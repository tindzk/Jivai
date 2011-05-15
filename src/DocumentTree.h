#import "Tree.h"
#import "String.h"
#import "Exception.h"

#define self DocumentTree

// @exc IllegalNesting

record(ref(Attr)) {
	CarrierString name;
	CarrierString value;
};

Array(ref(Attr), ref(Attrs));

record(ref(Node)) {
	Tree_Define(ref(Node));

	enum {
		ref(NodeType_Value),
		ref(NodeType_Tag)
	} type;

	CarrierString value;

	ref(Attrs) *attrs;
};

class {
	Tree tree;
	ref(Node) *node;

	size_t depth;
};

rsdef(self, New);
def(void, Destroy);
def(ref(Node) *, GetRoot);
def(void, AddTag, CarrierString value);
def(void, CloseTag);
def(void, AddValue, CarrierString value);
def(void, AddAttr, CarrierString value);
def(void, SetAttrValue, CarrierString value);
sdef(ref(Attr) *, GetAttr, ref(Node) *node, RdString name);
sdef(ref(Node) *, GetNodeByNames, ref(Node) *node, ...);
sdef(ref(Node) *, GetNodeByIds, ref(Node) *node, ...);

#undef self
