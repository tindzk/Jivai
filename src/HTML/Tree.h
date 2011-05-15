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
def(DocumentTree_Node *, GetRoot);
def(void, ProcessToken, HTML_TokenType type, RdString value);

#undef self
