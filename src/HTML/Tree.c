#import "Tree.h"

#define self HTML_Tree

rsdef(self, New) {
	return (self) {
		.tree  = Tree_New(Tree_DestroyNode_For(NULL, ref(DestroyNode))),
		.depth = 0,
		.node  = NULL
	};
}

def(void, Destroy) {
	Tree_Destroy(&this->tree);
}

def(void, DestroyNode, Tree_Node *_node) {
	ref(Node) *node = (void *) _node;

	CarrierString_Destroy(&node->value);

	if (node->type == ref(NodeType_Tag)) {
		each(item, node->attrs) {
			CarrierString_Destroy(&item->name);
			CarrierString_Destroy(&item->value);
		}

		scall(Attrs_Free, node->attrs);
	}
}

def(ref(Node) *, GetRoot) {
	return (ref(Node) *) &this->tree.root;
}

def(void, AddTag, CarrierString value) {
	this->node = Tree_AddNode(&this->tree, this->node);

	this->node->type  = ref(NodeType_Tag);
	this->node->value = value;
	this->node->attrs = scall(Attrs_New, 0);

	this->depth++;
}

def(void, CloseTag) {
	if (this->node->parent == NULL) {
		throw(IllegalNesting);
	}

	this->node = this->node->parent;

	this->depth--;
}

def(void, AddValue, CarrierString value) {
	ref(Node) *node = Tree_AddNode(&this->tree, this->node);

	node->type  = ref(NodeType_Value);
	node->attrs = NULL;
	node->value = value;
}

def(void, AddAttr, CarrierString value) {
	ref(Attr) item = {
		.name  = value,
		.value = CarrierString_New()
	};

	scall(Attrs_Push, &this->node->attrs, item);
}

def(void, SetAttrValue, CarrierString value) {
	ref(Attr) *attr = &this->node->attrs->buf[this->node->attrs->len - 1];
	attr->value = value;
}

def(void, ProcessToken, HTML_TokenType type, RdString value) {
	if (type == HTML_TokenType_TagStart) {
		call(AddTag, String_ToCarrier(RdString_Exalt(value)));
	} else if (type == HTML_TokenType_TagEnd) {
		call(CloseTag);
	} else if (type == HTML_TokenType_Value) {
		call(AddValue, String_ToCarrier(HTML_Entities_Decode(value)));
	} else if (type == HTML_TokenType_Data) {
		call(AddValue, String_ToCarrier(RdString_Exalt(value)));
	} else if (type == HTML_TokenType_AttrName
			|| type == HTML_TokenType_Option)
	{
		call(AddAttr, String_ToCarrier(RdString_Exalt(value)));
	} else if (type == HTML_TokenType_AttrValue) {
		call(SetAttrValue, HTML_Unescape(value));
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
