#import "Tree.h"

#define self Tree

rsdef(self, New, ref(DestroyNode) destroyNode) {
	return (self) {
		.destroyNode = destroyNode,
		.root.len    = 0,
		.root.buf    = NULL,
		.root.parent = NULL
	};
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

	fwd(i, node->len) {
		callback(this->destroyNode, node->buf[i]);
		call(FreeNodes, node->buf[i]);
		Pool_Free(Pool_GetInstance(), node->buf[i]);
	}

	Pool_Free(Pool_GetInstance(), node->buf);
	node->len = 0;
}

def(ref(Node) *, AddCustomNode, ref(Node) *node, size_t size) {
	if (node == NULL) {
		node = &this->root;
	}

	if (node->len == 0) {
		node->buf = Pool_Alloc(Pool_GetInstance(), sizeof(ref(Node) *));
	} else {
		node->buf = Pool_Realloc(Pool_GetInstance(),
			node->buf,
			(node->len + 1) * sizeof(ref(Node) *));
	}

	ref(Node) *res = node->buf[node->len] = Pool_Alloc(Pool_GetInstance(), size);

	res->len    = 0;
	res->buf    = NULL;
	res->parent = node;

	node->len++;

	return res;
}
