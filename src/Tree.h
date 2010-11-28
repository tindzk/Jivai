#import "Memory.h"

#undef self
#define self Tree

#define Tree_Define(type)  \
	/* Number of nodes. */ \
	size_t len;            \
                           \
	/* Children nodes. */  \
	struct type **buf;     \
                           \
	/* Parent nodes. */    \
	struct type *parent

record(ref(Node)) {
	Tree_Define(ref(Node));
};

typedef void (* ref(DestroyNode))(ref(Node) *);

class {
	ref(Node) root;
	ref(DestroyNode) destroyNode;
};

def(void, Init, ref(DestroyNode) destroyNode);
def(void, Destroy);
def(void, Reset);
def(void, FreeNodes, ref(Node) *node);
void* ref(AddCustomNode)(void *ptrNode, size_t size);

#define Tree_AddNode(node) \
	Tree_AddCustomNode(node, sizeof(typeof(*(node))))
