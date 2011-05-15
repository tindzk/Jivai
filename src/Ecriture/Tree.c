#import "Tree.h"

#define self Ecriture_Tree

rsdef(self, New) {
	return (self) {
		.tree = DocumentTree_New()
	};
}

def(void, Destroy) {
	DocumentTree_Destroy(&this->tree);
}

def(DocumentTree_Node *, GetRoot) {
	return DocumentTree_GetRoot(&this->tree);
}

def(void, ProcessToken, Ecriture_TokenType type, RdString value, size_t line) {
	if (type == Ecriture_TokenType_TagStart) {
		DocumentTree_AddTag(&this->tree, String_ToCarrier(RdString_Exalt(value)));
		DocumentTree_SetLine(&this->tree, line);
	} else if (type == Ecriture_TokenType_TagEnd) {
		DocumentTree_CloseTag(&this->tree);
	} else if (type == Ecriture_TokenType_Value) {
		DocumentTree_AddValue(&this->tree, String_ToCarrier(RdString_Exalt(value)));
	} else if (type == Ecriture_TokenType_Literal) {
		DocumentTree_AddValue(&this->tree, Ecriture_Unescape(value));
	} else if (type == Ecriture_TokenType_Option) {
		DocumentTree_AddAttr(&this->tree, CarrierString_New());
		DocumentTree_SetAttrValue(&this->tree, Ecriture_Unescape(value));
	} else if (type == Ecriture_TokenType_AttrName) {
		DocumentTree_AddAttr(&this->tree, Ecriture_Unescape(value));
	} else if (type == Ecriture_TokenType_AttrValue) {
		DocumentTree_SetAttrValue(&this->tree, Ecriture_Unescape(value));
	}
}
