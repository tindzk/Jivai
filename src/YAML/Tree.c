#import "Tree.h"

#define self YAML_Tree

static def(void, DestroyNode, __unused Tree_Node *node) { }

rsdef(self, New) {
	return (self) {
		.tree = Tree_New(Tree_DestroyNode_For(NULL, ref(DestroyNode))),
	};
}

def(void, Destroy) {
	Tree_Destroy(&this->tree);
}

def(ref(Node) *, GetRoot) {
	return (ref(Node) *) &this->tree.root;
}

def(void, Initialize) {
	assert(this->node == NULL);

	this->node = Tree_AddNode(&this->tree, this->node);

	this->node->type  = ref(NodeType_Node);
	this->node->name  = $("");
	this->node->value = $("");
}

def(void, ProcessToken, YAML_TokenType type, RdString value) {
	if (type == YAML_TokenType_Name) {
		ref(Node) *node = Tree_AddNode(&this->tree, this->node);

		node->type  = ref(NodeType_Section);
		node->name  = value;
		node->value = $("");
	} else if (type == YAML_TokenType_Value) {
		assert(this->node->len != 0);
		this->node->buf[this->node->len - 1]->type  = ref(NodeType_Item);
		this->node->buf[this->node->len - 1]->value = value;
	} else if (type == YAML_TokenType_Enter) {
		this->node = this->node->buf[this->node->len - 1];
	} else if (type == YAML_TokenType_Leave) {
		assert(this->node->parent != NULL);
		this->node = this->node->parent;
	}
}
