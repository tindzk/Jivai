#import "EventQueue.h"

/* Implements a FIFO queue using a linked list. */

#define self EventQueue

rsdef(self, New) {
	return (self) {
		.events = LinkedList_New()
	};
}

static def(void, DestroyEvent, void *item) {
	Memory_Destroy(item);
}

def(void, Destroy) {
	LinkedList_Destroy(&this->events,
		LinkedList_OnDestroy_For(this, ref(DestroyEvent)));
}

def(void, Enqueue, void *ptr, int flags) {
	ref(Event) *event = Memory_New(sizeof(ref(Event)));

	*event = (ref(Event)) {
		.ptr   = ptr,
		.flags = flags,
		.next  = NULL
	};

	LinkedList_Push(&this->events, event);
}

def(bool, HasEvents) {
	return this->events.first != NULL;
}

def(ref(Event), Pop) {
	assert(call(HasEvents));

	ref(Event) *event = this->events.first;
	ref(Event) stackEvent = *event;

	LinkedList_Pop(&this->events);

	call(DestroyEvent, event);

	return stackEvent;
}

def(bool, hasEvent, void *ptr, int flag) {
	LinkedList_Each(&this->events, node) {
		if (node->ptr == ptr && (node->flags & flag) != 0) {
			return true;
		}
	}

	return false;
}

def(void, Prune, void *ptr) {
	ref(Event) *node = this->events.first;
	ref(Event) *prev = NULL;
	ref(Event) *next;

	while (node != NULL) {
		next = node->next;

		if (node->ptr == ptr) {
			if (prev != NULL) {
				prev->next = next;
			} else {
				this->events.first = next;
			}

			call(DestroyEvent, node);
		} else {
			prev = node;
		}

		node = next;
	}
}
