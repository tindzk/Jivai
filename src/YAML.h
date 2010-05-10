#ifndef YAML_H
#define YAML_H

#include "Tree.h"
#include "String.h"
#include "Exception.h"
#include "StreamInterface.h"

Exception_Export(YAML_IllegalNestingException);

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
	void *p;
} YAML_Node;

typedef struct {
	StreamInterface *stream;
	void *context;

	Tree tree;
	YAML_Node *node;

	size_t depth;
	size_t depthWidth;
} YAML;

void YAML0(ExceptionManager *e);

void YAML_Init(YAML *this, size_t depthWidth, StreamInterface *stream, void *context);
void YAML_Destroy(YAML *this);
void YAML_DestroyNode(YAML_Node *node);
void YAML_AddSection(YAML *this, size_t depth, String s);
void YAML_AddItem(YAML *this, size_t depth, String key, String value);
void YAML_Parse(YAML *this);

#endif
