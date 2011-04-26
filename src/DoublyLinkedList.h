#import "LinkedList.h"

#define DoublyLinkedList_New() \
	{ NULL, NULL }

#define DoublyLinkedList_DeclareRef(type) \
	struct type *prev;                    \
	struct type *next

#define DoublyLinkedList_DeclareList(node, list) \
	record(list) {                               \
		node *first;                             \
		node *last;                              \
	}

#define DoublyLinkedList_InsertBefore(this, node, newNode) \
	do {                                                   \
		(newNode)->prev = (node)->prev;                    \
		(newNode)->next = node;                            \
		if ((node)->prev == NULL) {                        \
			(this)->first = newNode;                       \
		} else {                                           \
			(node)->prev->next = newNode;                  \
		}                                                  \
		(node)->prev = newNode;                            \
	} while (0)

#define DoublyLinkedList_InsertAfter(this, node, newNode) \
	do {                                                  \
		(newNode)->prev = node;                           \
		(newNode)->next = (node)->next;                   \
		if ((node)->next == NULL) {                       \
			(this)->last = newNode;                       \
		} else {                                          \
			(node)->next->prev = newNode;                 \
		}                                                 \
		(node)->next = newNode;                           \
	} while (0)

#define DoublyLinkedList_InsertBeginning(this, node)                  \
	do {                                                              \
		if ((this)->first == NULL) {                                  \
			(node)->prev = NULL;                                      \
			(node)->next = NULL;                                      \
			(this)->first = (this)->last = node;                      \
		} else {                                                      \
			DoublyLinkedList_InsertBefore(this, (this)->first, node); \
		}                                                             \
	} while (0)

#define DoublyLinkedList_InsertEnd(this, node) \
	do {                                       \
		(node)->prev = (this)->last;           \
		(node)->next = NULL;                   \
		if ((this)->first == NULL) {           \
			(this)->first = node;              \
		}                                      \
		if ((this)->last != NULL) {            \
			(this)->last->next = node;         \
		}                                      \
		(this)->last = node;                   \
	} while (0)

#define DoublyLinkedList_Remove(this, node)    \
	do {                                       \
		if ((node)->prev == NULL) {            \
			(this)->first = (node)->next;      \
		} else {                               \
			(node)->prev->next = (node)->next; \
		}                                      \
		if ((node)->next == NULL) {            \
			(this)->last = (node)->prev;       \
		} else {                               \
			(node)->next->prev = (node)->prev; \
		}                                      \
	} while (0)

#define DoublyLinkedList_Destroy(...) \
	LinkedList_Destroy(__VA_ARGS__)

#define DoublyLinkedList_Each(...) \
	LinkedList_Each(__VA_ARGS__)

#define DoublyLinkedList_ReverseEach(this, node) \
	for (typeof((this)->last) node = (this)->last; node != NULL; node = node->prev)
