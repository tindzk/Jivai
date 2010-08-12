#ifndef TREE_H
#define TREE_H

#include "Memory.h"

#define Tree_Define(type)  \
	/* Number of nodes. */ \
	size_t len;            \
                           \
	/* Children nodes. */  \
	struct type **nodes;   \
                           \
	/* Parent nodes. */    \
	struct type *parent

typedef struct _Tree_Node {
	Tree_Define(_Tree_Node);
} Tree_Node;

typedef void (* Tree_DestroyNode)(Tree_Node *);

typedef struct {
	Tree_Node root;
	Tree_DestroyNode destroyNode;
} Tree;

void Tree_Init(Tree *this, Tree_DestroyNode destroyNode);
void Tree_Destroy(Tree *this);
void Tree_Reset(Tree *this);
void Tree_FreeNodes(Tree *this, Tree_Node *node);
void* Tree_AddCustomNode(Tree_Node *node, size_t size);
Tree_Node* Tree_AddNode(Tree_Node *node);

#define Tree_AddNode(node, size) \
	Tree_AddCustomNode((Tree_Node *) (node), size)

#endif
