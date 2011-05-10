#import "String.h"
#import "Memory.h"

#define self Tree

#define Tree_Define(type)  \
	/* Number of nodes. */ \
	size_t len;            \
                           \
	/* Children nodes. */  \
	struct type **buf;     \
                           \
	/* Parent node. */     \
	struct type *parent

record(ref(Node)) {
	Tree_Define(ref(Node));
};

Callback(ref(DestroyNode), void, ref(Node) *);

class {
	ref(Node) root;
	ref(DestroyNode) destroyNode;
};

rsdef(self, New, ref(DestroyNode) destroyNode);
def(void, Destroy);
def(void, Reset);
def(void, FreeNodes, ref(Node) *node);
def(ref(Node) *, AddCustomNode, ref(Node) *node, size_t size);

#define Tree_AddNode($this, node) \
	(typeof(node)) Tree_AddCustomNode($this, (Tree_Node *) (node), sizeof(typeof(*(node))))

#undef self
