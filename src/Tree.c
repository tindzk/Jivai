#import "Tree.h"
#import "App.h"

def(void, Init, ref(DestroyNode) destroyNode) {
	this->destroyNode = destroyNode;

	this->root.len    = 0;
	this->root.buf    = NULL;
	this->root.parent = NULL;
}

def(void, Destroy) {
	call(FreeNodes, &this->root);
}

def(void, Reset) {
	call(FreeNodes, &this->root);

	this->root.len    = 0;
	this->root.buf    = NULL;
	this->root.parent = NULL;
}

def(void, FreeNodes, ref(Node) *node) {
	if (node->len == 0) {
		return;
	}

	for (size_t i = 0; i < node->len; i++) {
		this->destroyNode(node->buf[i]);
		call(FreeNodes, node->buf[i]);
		Memory_Free(node->buf[i]);
	}

	Memory_Free(node->buf);
	node->len = 0;
}

void* ref(AddCustomNode)(void *ptrNode, size_t size) {
	ref(Node) *node = ptrNode;

	if (node->len == 0) {
		node->buf = New(ref(Node) *);
	} else {
		node->buf = Memory_Realloc(
			node->buf,
			(node->len + 1) * sizeof(ref(Node) *));
	}

	ref(Node) *res = node->buf[node->len] = Memory_Alloc(size);

	res->len    = 0;
	res->buf    = NULL;
	res->parent = node;

	node->len++;

	return res;
}
