#import "Tree.h"

def(void, Init) {
	Tree_Init(&this->tree, (void *) ref(DestroyNode));

	this->node  = (ref(Node) *) &this->tree.root;
	this->depth = 0;
}

def(void, Destroy) {
	Tree_Destroy(&this->tree);
}

sdef(void, DestroyNode, ref(Node) *node) {
	String_Destroy(&node->value);

	if (node->type == ref(NodeType_Tag)) {
		foreach (item, node->attrs) {
			String_Destroy(&item->name);
			String_Destroy(&item->value);
		}

		Array_Destroy(node->attrs);
	}
}

def(ref(Node) *, GetRoot) {
	return (ref(Node) *) &this->tree.root;
}

def(void, ProcessToken, HTML_Tokenizer_TokenType type, String value) {
	if (type == HTML_Tokenizer_TokenType_TagEnd) {
		if (this->node->parent == NULL) {
			throw(excIllegalNesting);
		}

		this->node = this->node->parent;

		this->depth--;
	} else if (type == HTML_Tokenizer_TokenType_TagStart) {
		this->node = Tree_AddNode(this->node);

		this->node->value = String_Clone(value);
		this->node->type  = ref(NodeType_Tag);

		Array_Init(this->node->attrs, 0);

		this->depth++;
	} else if (type == HTML_Tokenizer_TokenType_Value) {
		ref(Node) *node = Tree_AddNode(this->node);

		node->type  = ref(NodeType_Value);
		node->value = HTML_Entities_Decode(value);
		node->attrs = NULL;
	} else if (type == HTML_Tokenizer_TokenType_AttrName
			|| type == HTML_Tokenizer_TokenType_Option) {
		ref(Attr) item;

		item.name  = String_Clone(value);
		item.value = HeapString(0);

		Array_Push(this->node->attrs, item);
	} else if (type == HTML_Tokenizer_TokenType_AttrValue) {
		ref(Attr) *attr = &this->node->attrs->buf[this->node->attrs->len - 1];
		attr->value = String_Clone(value);
	}
}

sdef(ref(Attr) *, GetAttr, ref(Node) *node, String name) {
	if (node->type == ref(NodeType_Tag)) {
		for (size_t i = 0; i < node->attrs->len; i++) {
			if (String_Equals(node->attrs->buf[i].name, name)) {
				return &node->attrs->buf[i];
			}
		}
	}

	return NULL;
}

sdef(ref(Node) *, GetNodeByNames, ref(Node) *node, ...) {
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
			if (node->buf[i]->type == ref(NodeType_Tag)) {
				if (String_Equals(*s, node->buf[i]->value)) {
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

sdef(void, Foreach, ref(Node) *node, void (^cb)(ref(Node) *)) {
	for (size_t i = 0; i < node->len; i++) {
		cb(node->buf[i]);
	}
}
