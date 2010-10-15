#import "Tree.h"

void Tree_Init(Tree *this, Tree_DestroyNode destroyNode) {
	this->destroyNode = destroyNode;

	this->root.len = 0;
	this->root.nodes = NULL;
	this->root.parent = NULL;
}

void Tree_Destroy(Tree *this) {
	Tree_FreeNodes(this, &this->root);
}

void Tree_Reset(Tree *this) {
	Tree_FreeNodes(this, &this->root);

	this->root.len = 0;
	this->root.nodes = NULL;
	this->root.parent = NULL;
}

void Tree_FreeNodes(Tree *this, Tree_Node *node) {
	if (node->len == 0) {
		return;
	}

	for (size_t i = 0; i < node->len; i++) {
		this->destroyNode(node->nodes[i]);
		Tree_FreeNodes(this, node->nodes[i]);
		Memory_Free(node->nodes[i]);
	}

	Memory_Free(node->nodes);
	node->len = 0;
}

void* Tree_AddCustomNode(void *ptrNode, size_t size) {
	Tree_Node *node = ptrNode;

	if (node->len == 0) {
		node->nodes = New(Tree_Node *);
	} else {
		node->nodes = Memory_Realloc(
			node->nodes,
			(node->len + 1) * sizeof(Tree_Node *));
	}

	Tree_Node *res = node->nodes[node->len] = Memory_Alloc(size);

	res->len = 0;
	res->nodes = NULL;
	res->parent = node;

	node->len++;

	return res;
}
