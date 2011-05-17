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

def(void, Initialize) {
	DocumentTree_Initialize(&this->tree);
}

def(DocumentTree_Node *, GetRoot) {
	return DocumentTree_GetRoot(&this->tree);
}

def(void, ProcessToken, XML_TokenType type, RdString value) {
	assert(type != XML_TokenType_Data);

	if (type == XML_TokenType_TagStart) {
		DocumentTree_AddTag(&this->tree, String_ToCarrier(RdString_Exalt(value)));
	} else if (type == XML_TokenType_TagEnd) {
		DocumentTree_CloseTag(&this->tree);
	} else if (type == XML_TokenType_Value) {
		DocumentTree_AddValue(&this->tree, String_ToCarrier(HTML_Entities_Decode(value)));
	} else if (type == XML_TokenType_AttrName
			|| type == XML_TokenType_Option)
	{
		DocumentTree_AddAttr(&this->tree, String_ToCarrier(RdString_Exalt(value)));
	} else if (type == XML_TokenType_AttrValue) {
		DocumentTree_SetAttrValue(&this->tree, XML_Unescape(value));
	} else if (type == XML_TokenType_Comment) {
		DocumentTree_AddComment(&this->tree, String_ToCarrier(RdString_Exalt(value)));
	}
}

sdef(void, BuildTokens, XML_OnToken onToken, DocumentTree_Node *node) {
	fwd(i, node->len) {
		DocumentTree_Node *cur = node->buf[i];

		if (cur->type == DocumentTree_NodeType_Tag) {
			callback(onToken, XML_TokenType_TagStart, cur->value.rd);

			fwd(j, cur->attrs->len) {
				RdString attrName  = cur->attrs->buf[j].name.rd;
				RdString attrValue = cur->attrs->buf[j].value.rd;

				if (attrName.len == 0) {
					CarrierString value = HTML_Escape(attrValue);
					callback(onToken, XML_TokenType_Option, value.rd);
					CarrierString_Destroy(&value);
				} else {
					CarrierString name  = HTML_Escape(attrName);
					CarrierString value = HTML_Escape(attrValue);

					callback(onToken, XML_TokenType_AttrName,  name.rd);
					callback(onToken, XML_TokenType_AttrValue, value.rd);

					CarrierString_Destroy(&value);
					CarrierString_Destroy(&name);
				}
			}

			scall(BuildTokens, onToken, cur);

			callback(onToken, XML_TokenType_TagEnd,
				(cur->len == 0 && !HTML_IsNestable(cur->value.rd))
					? $("") /* XML end tag. */
					: cur->value.rd);
		} else if (node->buf[i]->type == DocumentTree_NodeType_Comment) {
			CarrierString str = String_Replace(cur->value.rd, $("-->"), $(""));
			callback(onToken, XML_TokenType_Comment, str.rd);
			CarrierString_Destroy(&str);
		} else if (node->buf[i]->type == DocumentTree_NodeType_Value) {
			String value = HTML_Entities_Encode(cur->value.rd);
			callback(onToken, XML_TokenType_Value, value.rd);
			String_Destroy(&value);
		} else if (node->buf[i]->type == DocumentTree_NodeType_Node) { /* Root node. */
			scall(BuildTokens, onToken, cur);
		}
	}
}
