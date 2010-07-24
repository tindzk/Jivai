#ifndef HTML_TREE_H
#define HTML_TREE_H

#include "Tokenizer.h"

#include "../Tree.h"
#include "../Block.h"
#include "../Array.h"
#include "../String.h"
#include "../Exception.h"

Exception_Export(HTML_Tree_IllegalNestingException);

typedef struct {
	String name;
	String value;
} HTML_Tree_Attr;

typedef struct _HTML_Tree_Node {
	Tree_Define(_HTML_Tree_Node);

	enum {
		HTML_Tree_NodeType_Value,
		HTML_Tree_NodeType_Tag
	} type;

	String value;

	Array(HTML_Tree_Attr, *attrs);
} HTML_Tree_Node;

typedef struct {
	Tree tree;
	HTML_Tree_Node *node;

	size_t depth;
} HTML_Tree;

void HTML_Tree0(ExceptionManager *e);

void HTML_Tree_Init(HTML_Tree *this);
void HTML_Tree_Destroy(HTML_Tree *this);
void HTML_Tree_DestroyNode(HTML_Tree_Node *node);
HTML_Tree_Node* HTML_Tree_GetRoot(HTML_Tree *this);
void HTML_Tree_ProcessToken(HTML_Tree *this, HTML_Tokenizer_TokenType type, String value);
HTML_Tree_Attr* HTML_Tree_GetAttr(HTML_Tree_Node *node, String name);
HTML_Tree_Node* HTML_Tree_GetNodeByNames(HTML_Tree_Node *node, ...);
HTML_Tree_Node* HTML_Tree_GetNodeByIds(HTML_Tree_Node *node, ...);
void HTML_Tree_Foreach(HTML_Tree_Node *node, void (^cb)(HTML_Tree_Node *));

#endif
