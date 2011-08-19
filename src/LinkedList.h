#import "Macros.h"

Callback(LinkedList_OnDestroy, void, void *item);

#define LinkedList_New() \
	{ null, null }

#define LinkedList_DeclareRef(type) \
	struct type *next;

#define LinkedList_DeclareList(node, list) \
	record(list) {                         \
		node *first;                       \
		node *last;                        \
	}

#define LinkedList_Push(this, node) \
	do {                                 \
		(node)->next = null;             \
		if ((this)->first == null) {     \
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
		while (node != null) {                      \
			next = node->next;                      \
			callback(freeNode, node);               \
			node = next;                            \
		}                                           \
		(this)->first = null;                       \
		(this)->last  = null;                       \
	} while (0)

#define LinkedList_Each(this, node) \
	for (typeof((this)->first) node = (this)->first; node != null; node = node->next)

#define LinkedList_safeEach(this, node)                     \
	for (typeof((this)->first) next, node = (this)->first;  \
			node != null && ({ next = node->next; true; }); \
			node = next)
