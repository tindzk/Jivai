#import "Tree.h"

#define self HTML_Tree

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

def(void, ProcessToken, HTML_TokenType type, RdString value) {
	if (type == HTML_TokenType_TagStart) {
		DocumentTree_AddTag(&this->tree, String_ToCarrier(RdString_Exalt(value)));
	} else if (type == HTML_TokenType_TagEnd) {
		DocumentTree_CloseTag(&this->tree);
	} else if (type == HTML_TokenType_Value) {
		DocumentTree_AddValue(&this->tree, String_ToCarrier(HTML_Entities_Decode(value)));
	} else if (type == HTML_TokenType_Data) {
		DocumentTree_AddValue(&this->tree, String_ToCarrier(RdString_Exalt(value)));
	} else if (type == HTML_TokenType_AttrName
			|| type == HTML_TokenType_Option)
	{
		DocumentTree_AddAttr(&this->tree, String_ToCarrier(RdString_Exalt(value)));
	} else if (type == HTML_TokenType_AttrValue) {
		DocumentTree_SetAttrValue(&this->tree, HTML_Unescape(value));
	}
}
