#import "DocumentTree.h"

#define self DocumentTree

static def(void, DestroyNode, Tree_Node *_node);

rsdef(self, New) {
	return (self) {
		.tree = Tree_New(Tree_DestroyNode_For(NULL, ref(DestroyNode)))
	};
}

def(void, Destroy) {
	Tree_Destroy(&this->tree);
}

static def(void, DestroyNode, Tree_Node *_node) {
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

def(void, Initialize) {
	assert(this->node == NULL);

	this->node = Tree_AddNode(&this->tree, this->node);

	this->node->type  = ref(NodeType_Node);
	this->node->value = CarrierString_New();
	this->node->attrs = NULL;
	this->node->line  = 0;
}

def(void, AddTag, CarrierString value) {
	this->node = Tree_AddNode(&this->tree, this->node);

	this->node->type  = ref(NodeType_Tag);
	this->node->value = value;
	this->node->attrs = scall(Attrs_New, 0);
	this->node->line  = 0;
}

def(void, CloseTag) {
	if (this->node->parent == NULL) {
		throw(IllegalNesting);
	}

	this->node = this->node->parent;
}

def(void, SetLine, size_t line) {
	assert(line > 0);
	assert(this->node != NULL);

	this->node->line = line;
}

def(void, AddValue, CarrierString value) {
	assert(this->node != NULL);

	ref(Node) *node = Tree_AddNode(&this->tree, this->node);

	node->type  = ref(NodeType_Value);
	node->attrs = NULL;
	node->value = value;
}

def(void, AddComment, CarrierString value) {
	assert(this->node != NULL);

	ref(Node) *node = Tree_AddNode(&this->tree, this->node);

	node->type  = ref(NodeType_Comment);
	node->attrs = NULL;
	node->value = value;
}

def(void, AddAttr, CarrierString value) {
	assert(this->node != NULL);

	scall(Attrs_Push, &this->node->attrs, (ref(Attr)) {
		.name  = value,
		.value = CarrierString_New()
	});
}

def(void, SetAttrValue, CarrierString value) {
	assert(this->node != NULL);

	ref(Attr) *attr = &this->node->attrs->buf[this->node->attrs->len - 1];
	attr->value = value;
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
