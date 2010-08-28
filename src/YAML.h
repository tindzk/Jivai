#import "Tree.h"
#import "String.h"
#import "Exception.h"
#import "StreamInterface.h"

#undef self
#define self YAML

typedef enum {
	YAML_NodeType_Node,
	YAML_NodeType_Section,
	YAML_NodeType_Item
} YAML_NodeType;

typedef struct {
	String name;
} YAML_Section;

typedef struct {
	String key;
	String value;
} YAML_Item;

typedef struct _YAML_Node {
	Tree_Define(_YAML_Node);

	YAML_NodeType type;
	char data[0];
} YAML_Node;

#define YAML_Section(node) \
	((YAML_Section *) &(node)->data)

#define YAML_Item(node) \
	((YAML_Item *) &(node)->data)

typedef struct {
	StreamInterface *stream;
	void *context;

	size_t line;

	Tree tree;
	YAML_Node *node;

	size_t depth;
	size_t depthWidth;
} YAML;

extern size_t Modules_YAML;

void YAML0(ExceptionManager *e);

void YAML_Init(YAML *this, size_t depthWidth, StreamInterface *stream, void *context);
void YAML_Destroy(YAML *this);
void YAML_DestroyNode(YAML_Node *node);
YAML_Node* YAML_GetRoot(YAML *this);
size_t YAML_GetLine(YAML *this);
void* YAML_Store(YAML *this, size_t depth, YAML_NodeType type, size_t size);
void YAML_AddSection(YAML *this, size_t depth, String s);
void YAML_AddItem(YAML *this, size_t depth, String key, String value);
void YAML_Parse(YAML *this);
