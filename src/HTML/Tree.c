#import "Tree.h"

static ExceptionManager *exc;

void HTML_Tree0(ExceptionManager *e) {
	exc = e;
}

void HTML_Tree_Init(HTML_Tree *this) {
	Tree_Init(&this->tree, (void *) &HTML_Tree_DestroyNode);

	this->node = (HTML_Tree_Node *) &this->tree.root;
	this->depth = 0;
}

void HTML_Tree_Destroy(HTML_Tree *this) {
	Tree_Destroy(&this->tree);
}

void HTML_Tree_DestroyNode(HTML_Tree_Node *node) {
	String_Destroy(&node->value);

	if (node->type == HTML_Tree_NodeType_Tag) {
		Array_Foreach(node->attrs, ^(HTML_Tree_Attr *item) {
			String_Destroy(&item->name);
			String_Destroy(&item->value);
		});

		Array_Destroy(node->attrs);
	}
}

HTML_Tree_Node* HTML_Tree_GetRoot(HTML_Tree *this) {
	return (HTML_Tree_Node *) &this->tree.root;
}

void HTML_Tree_ProcessToken(HTML_Tree *this, HTML_Tokenizer_TokenType type, String value) {
	if (type == HTML_Tokenizer_TokenType_TagEnd) {
		if (this->node->parent == NULL) {
			throw(exc, excIllegalNesting);
		}

		this->node = this->node->parent;

		this->depth--;
	} else if (type == HTML_Tokenizer_TokenType_TagStart) {
		this->node = Tree_AddNode(this->node);

		this->node->value = String_Clone(value);
		this->node->type  = HTML_Tree_NodeType_Tag;

		Array_Init(this->node->attrs, 0);

		this->depth++;
	} else if (type == HTML_Tokenizer_TokenType_Value) {
		HTML_Tree_Node *node = Tree_AddNode(this->node);

		node->type  = HTML_Tree_NodeType_Value;
		node->value = HTML_Entities_Decode(value);
		node->attrs = NULL;
	} else if (type == HTML_Tokenizer_TokenType_AttrName
			|| type == HTML_Tokenizer_TokenType_Option) {
		HTML_Tree_Attr item;

		item.name  = String_Clone(value);
		item.value = HeapString(0);

		Array_Push(this->node->attrs, item);
	} else if (type == HTML_Tokenizer_TokenType_AttrValue) {
		HTML_Tree_Attr *attr = &this->node->attrs->buf[this->node->attrs->len - 1];
		attr->value = String_Clone(value);
	}
}

HTML_Tree_Attr* HTML_Tree_GetAttr(HTML_Tree_Node *node, String name) {
	if (node->type == HTML_Tree_NodeType_Tag) {
		for (size_t i = 0; i < node->attrs->len; i++) {
			if (String_Equals(node->attrs->buf[i].name, name)) {
				return &node->attrs->buf[i];
			}
		}
	}

	return NULL;
}

HTML_Tree_Node* HTML_Tree_GetNodeByNames(HTML_Tree_Node *node, ...) {
	String *s;
	bool found;
	VarArg argptr;

	VarArg_Start(argptr, node);

	while (true) {
		s = VarArg_Get(argptr, String *);

		if (s == NULL) {
			break;
		}

		found = false;

		for (size_t i = 0; i < node->len; i++) {
			if (node->nodes[i]->type == HTML_Tree_NodeType_Tag) {
				if (String_Equals(*s, node->nodes[i]->value)) {
					node = node->nodes[i];
					found = true;
					break;
				}
			}
		}

		if (!found) {
			break;
		}
	}

	VarArg_End(argptr);

	return found
		? node
		: NULL;
}

HTML_Tree_Node* HTML_Tree_GetNodeByIds(HTML_Tree_Node *node, ...) {
	int id;
	VarArg argptr;
	bool found = true;

	VarArg_Start(argptr, node);

	while (true) {
		if ((id = VarArg_Get(argptr, int)) < 0) {
			break;
		}

		if ((size_t) id >= node->len) {
			found = false;
			break;
		}

		node = node->nodes[id];
	}

	VarArg_End(argptr);

	return found
		? node
		: NULL;
}

void HTML_Tree_Foreach(HTML_Tree_Node *node, void (^cb)(HTML_Tree_Node *)) {
	for (size_t i = 0; i < node->len; i++) {
		cb(node->nodes[i]);
	}
}
