#include "Tree.h"

void Tree_Init(Tree *this, Tree_DestroyNode destroyNode) {
	this->destroyNode = destroyNode;

	this->root.cnt = 0;
	this->root.nodes = NULL;
	this->root.parent = NULL;
}

void Tree_Destroy(Tree *this) {
	Tree_FreeNodes(this, &this->root);
}

void Tree_Reset(Tree *this) {
	Tree_FreeNodes(this, &this->root);

	this->root.cnt = 0;
	this->root.nodes = NULL;
	this->root.parent = NULL;
}

void Tree_FreeNodes(Tree *this, Tree_Node *node) {
	if (node->cnt == 0) {
		return;
	}

	for (size_t i = 0; i < node->cnt; i++) {
		this->destroyNode(node->nodes[i]);
		Tree_FreeNodes(this, node->nodes[i]);
		Memory_Free(node->nodes[i]);
	}

	Memory_Free(node->nodes);
	node->cnt = 0;
}

void* Tree_AddCustomNode(Tree_Node *node, size_t size) {
	if (node->cnt == 0) {
		node->nodes = New(Tree_Node *);
	} else {
		node->nodes = Memory_Realloc(
			node->nodes,
			(node->cnt + 1) * sizeof(Tree_Node *));
	}

	Tree_Node *res = node->nodes[node->cnt] = Memory_Alloc(size);

	res->cnt = 0;
	res->nodes = NULL;
	res->parent = node;

	node->cnt++;

	return res;
}
