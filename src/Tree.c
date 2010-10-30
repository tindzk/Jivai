#import "Tree.h"
#import "App.h"

def(void, Init, ref(DestroyNode) destroyNode) {
	this->destroyNode = destroyNode;

	this->root.len    = 0;
	this->root.nodes  = NULL;
	this->root.parent = NULL;
}

def(void, Destroy) {
	call(FreeNodes, &this->root);
}

def(void, Reset) {
	call(FreeNodes, &this->root);

	this->root.len    = 0;
	this->root.nodes  = NULL;
	this->root.parent = NULL;
}

def(void, FreeNodes, ref(Node) *node) {
	if (node->len == 0) {
		return;
	}

	for (size_t i = 0; i < node->len; i++) {
		this->destroyNode(node->nodes[i]);
		call(FreeNodes, node->nodes[i]);
		Memory_Free(node->nodes[i]);
	}

	Memory_Free(node->nodes);
	node->len = 0;
}

void* ref(AddCustomNode)(void *ptrNode, size_t size) {
	ref(Node) *node = ptrNode;

	if (node->len == 0) {
		node->nodes = New(ref(Node) *);
	} else {
		node->nodes = Memory_Realloc(
			node->nodes,
			(node->len + 1) * sizeof(ref(Node) *));
	}

	ref(Node) *res = node->nodes[node->len] = Memory_Alloc(size);

	res->len    = 0;
	res->nodes  = NULL;
	res->parent = node;

	node->len++;

	return res;
}
