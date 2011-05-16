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

def(void, Initialize) {
	DocumentTree_Initialize(&this->tree);
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

sdef(void, BuildTokens, Ecriture_OnBuildToken onToken, DocumentTree_Node *node) {
	fwd(i, node->len) {
		DocumentTree_Node *cur = node->buf[i];

		if (cur->type == DocumentTree_NodeType_Tag) {
			callback(onToken, Ecriture_TokenType_TagStart, cur->value.rd);

			fwd(j, cur->attrs->len) {
				RdString attrName  = cur->attrs->buf[j].name.rd;
				RdString attrValue = cur->attrs->buf[j].value.rd;

				if (attrName.len == 0) {
					CarrierString value = Ecriture_Escape(attrValue,
						Ecriture_TokenType_Option);
					callback(onToken, Ecriture_TokenType_Option, value.rd);
					CarrierString_Destroy(&value);
				} else {
					CarrierString name = Ecriture_Escape(attrName,
						Ecriture_TokenType_AttrName);
					CarrierString value = Ecriture_Escape(attrValue,
						Ecriture_TokenType_AttrValue);

					callback(onToken, Ecriture_TokenType_AttrName,  name.rd);
					callback(onToken, Ecriture_TokenType_AttrValue, value.rd);

					CarrierString_Destroy(&value);
					CarrierString_Destroy(&name);
				}
			}

			scall(BuildTokens, onToken, cur);

			callback(onToken, Ecriture_TokenType_TagEnd, $(""));
		} else if (node->buf[i]->type == DocumentTree_NodeType_Value) {
			CarrierString value = Ecriture_Escape(cur->value.rd,
				Ecriture_TokenType_Value);
			callback(onToken, Ecriture_TokenType_Value, value.rd);
			CarrierString_Destroy(&value);
		} else if (node->buf[i]->type == DocumentTree_NodeType_Node) { /* Root node. */
			scall(BuildTokens, onToken, cur);
		}
	}
}
