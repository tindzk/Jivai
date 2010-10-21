#import "Tree.h"
#import "String.h"
#import "Exception.h"
#import "StreamInterface.h"

#undef self
#define self Typography

typedef struct {
	String value;
} Typography_Text;

typedef struct {
	String name;
	String options;
} Typography_Item;

typedef struct _Typography_Node {
	Tree_Define(_Typography_Node);

	size_t line;

	enum {
		Typography_NodeType_Item,
		Typography_NodeType_Text
	} type;

	char data[0];
} Typography_Node;

#define Typography_Text(node) \
	((Typography_Text *) &(node)->data)

#define Typography_Item(node) \
	((Typography_Item *) &(node)->data)

typedef struct {
	StreamInterface *stream;
	void *context;

	size_t line;

	Tree tree;
	Typography_Node *node;
} Typography;

void Typography0(ExceptionManager *e);
void Typography_Init(Typography *this, StreamInterface *stream, void *context);
void Typography_Destroy(Typography *this);
void Typography_DestroyNode(Typography_Node *node);
Typography_Node* Typography_GetRoot(Typography *this);
void Typography_Parse(Typography *this);
