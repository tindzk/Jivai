#import "Entities.h"

#import "../XML.h"
#import "../HTML.h"
#import "../String.h"
#import "../DocumentTree.h"

#define self HTML_Tree

class {
	DocumentTree tree;
};

rsdef(self, New);
def(void, Destroy);
def(void, Initialize);
def(DocumentTree_Node *, GetRoot);
def(void, ProcessToken, XML_TokenType type, RdString value);
sdef(void, BuildTokens, XML_OnToken onToken, DocumentTree_Node *node);

#undef self
