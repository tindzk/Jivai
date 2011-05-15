#import "../String.h"
#import "../Ecriture.h"
#import "../DocumentTree.h"

#define self Ecriture_Tree

class {
	DocumentTree tree;
};

rsdef(self, New);
def(void, Destroy);
def(DocumentTree_Node *, GetRoot);
def(void, ProcessToken, Ecriture_TokenType type, RdString value, size_t line);

#undef self
