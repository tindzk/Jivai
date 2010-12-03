#import "Tree.h"
#import "String.h"
#import "Exception.h"
#import "StreamInterface.h"

#define self Typography

record(ref(Text)) {
	String value;
};

record(ref(Item)) {
	String name;
	String options;
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

#define Typography_Text(node) \
	((Typography_Text *) &(node)->data)

#define Typography_Item(node) \
	((Typography_Item *) &(node)->data)

class {
	StreamInterface *stream;
	void *context;

	size_t line;

	Tree tree;
	ref(Node) *node;
};

def(void, Init);
def(void, Destroy);
void ref(DestroyNode)(ref(Node) *node);
def(ref(Node) *, GetRoot);
def(void, Parse, StreamInterface *stream, void *context);

#undef self
