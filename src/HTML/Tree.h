#import "Entities.h"

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
def(void, ProcessToken, HTML_TokenType type, RdString value);
sdef(void, BuildTokens, HTML_OnToken onToken, DocumentTree_Node *node);

#undef self
