#import "Macros.h"

Callback(LinkedList_OnDestroy, void, void *item);

#define LinkedList_New() \
	{ NULL, NULL }

#define LinkedList_DeclareRef(type) \
	struct type *next;

#define LinkedList_DeclareList(node, list) \
	record(list) {                         \
		node *first;                       \
		node *last;                        \
	}

#define LinkedList_Push(this, node) \
	do {                                 \
		(node)->next = NULL;             \
		if ((this)->first == NULL) {     \
			(this)->first = node;        \
			(this)->last  = node;        \
		} else {                         \
			(this)->last->next = node;   \
			(this)->last = node;         \
		}                                \
	} while (0)

#define LinkedList_Pop(this)                 \
	do {                                     \
		(this)->first = (this)->first->next; \
	} while (0)

#define LinkedList_Destroy(this, freeNode)          \
	do {                                            \
		typeof((this)->first) node = (this)->first; \
		typeof((this)->first) next;                 \
		while (node != NULL) {                      \
			next = node->next;                      \
			callback(freeNode, node);               \
			node = next;                            \
		}                                           \
		(this)->first = NULL;                       \
		(this)->last  = NULL;                       \
	} while (0)

#define LinkedList_Each(this, node) \
	for (typeof((this)->first) node = (this)->first; node != NULL; node = node->next)
