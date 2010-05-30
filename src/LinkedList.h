#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#define LinkedList_New() \
	{ NULL, NULL }

#define LinkedList_Init(this) \
	do {                      \
		(this)->first = NULL; \
		(this)->last  = NULL; \
	} while (0)

#define LinkedList_DeclareRef(type) \
	struct type *next;

#define LinkedList_DeclareList(node, list) \
	typedef struct {                       \
		node *first;                       \
		node *last;                        \
	} list

#define LinkedList_InsertEnd(this, node) \
	do {                               \
		(node)->next = NULL;           \
		if ((this)->first == NULL) {   \
			(this)->first = node;      \
			(this)->last  = node;      \
		} else {                       \
			(this)->last->next = node; \
			(this)->last = node;       \
		}                              \
	} while (0)

#define LinkedList_Destroy(this, freeNode)          \
	do {                                            \
		typeof((this)->first) node = (this)->first; \
		typeof((this)->first) next;                 \
		while (node != NULL) {                      \
			next = node->next;                      \
			freeNode(node);                         \
			node = next;                            \
		}                                           \
		(this)->first = NULL;                       \
		(this)->last  = NULL;                       \
	} while(0)

#define LinkedList_Foreach(this, node) \
	for (typeof((this)->first) node = (this)->first; node != NULL; node = node->next)

#endif
