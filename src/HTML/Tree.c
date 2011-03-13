#import "Tree.h"

#define self HTML_Tree

rsdef(self, New) {
	return (self) {
		.tree  = Tree_New((void *) ref(DestroyNode)),
		.depth = 0,
		.node  = NULL
	};
}

def(void, Destroy) {
	Tree_Destroy(&this->tree);
}

sdef(void, DestroyNode, ref(Node) *node) {
	String_Destroy(&node->value);

	if (node->type == ref(NodeType_Tag)) {
		each(item, node->attrs) {
			String_Destroy(&item->name);
			String_Destroy(&item->value);
		}

		scall(Attrs_Free, node->attrs);
	}
}

def(ref(Node) *, GetRoot) {
	return (ref(Node) *) &this->tree.root;
}

def(void, ProcessToken, HTML_Tokenizer_TokenType type, RdString value) {
	if (type == HTML_Tokenizer_TokenType_TagEnd) {
		if (this->node->parent == NULL) {
			throw(IllegalNesting);
		}

		this->node = this->node->parent;

		this->depth--;
	} else if (type == HTML_Tokenizer_TokenType_TagStart) {
		this->node = Tree_AddNode(&this->tree, this->node);

		this->node->type  = ref(NodeType_Tag);
		this->node->value = String_Clone(value);
		this->node->attrs = scall(Attrs_New, 0);

		this->depth++;
	} else if (type == HTML_Tokenizer_TokenType_Value) {
		ref(Node) *node = Tree_AddNode(&this->tree, this->node);

		node->type  = ref(NodeType_Value);
		node->value = HTML_Entities_Decode(value);
		node->attrs = NULL;
	} else if (type == HTML_Tokenizer_TokenType_AttrName
			|| type == HTML_Tokenizer_TokenType_Option)
	{
		ref(Attr) item = {
			.name  = String_Clone(value),
			.value = String_New(0)
		};

		scall(Attrs_Push, &this->node->attrs, item);
	} else if (type == HTML_Tokenizer_TokenType_AttrValue) {
		ref(Attr) *attr = &this->node->attrs->buf[this->node->attrs->len - 1];
		attr->value = String_Clone(value);
	}
}

sdef(ref(Attr) *, GetAttr, ref(Node) *node, RdString name) {
	if (node->type == ref(NodeType_Tag)) {
		fwd(i, node->attrs->len) {
			if (String_Equals(node->attrs->buf[i].name.rd, name)) {
				return &node->attrs->buf[i];
			}
		}
	}

	return NULL;
}

sdef(ref(Node) *, GetNodeByNames, ref(Node) *node, ...) {
	bool found;
	RdString *s;
	VarArg argptr;

	VarArg_Start(argptr, node);

	while (true) {
		s = VarArg_Get(argptr, RdString *);

		if (s == NULL) {
			break;
		}

		found = false;

		fwd(i, node->len) {
			if (node->buf[i]->type == ref(NodeType_Tag)) {
				if (String_Equals(*s, node->buf[i]->value.rd)) {
					node = node->buf[i];
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

sdef(ref(Node) *, GetNodeByIds, ref(Node) *node, ...) {
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

		node = node->buf[id];
	}

	VarArg_End(argptr);

	return found
		? node
		: NULL;
}
