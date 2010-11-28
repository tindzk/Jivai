#import "Tree.h"
#import "String.h"
#import "Exception.h"
#import "StreamInterface.h"

#undef self
#define self Typography

typedef struct {
	String value;
} ref(Text);

typedef struct {
	String name;
	String options;
} ref(Item);

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

ExtendClass(self);

def(void, Init);
def(void, Destroy);
void ref(DestroyNode)(ref(Node) *node);
def(ref(Node) *, GetRoot);
def(void, Parse, StreamInterface *stream, void *context);
