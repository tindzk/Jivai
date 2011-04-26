#import "String.h"
#import "LinkedList.h"

#define self EventQueue

record(ref(Event)) {
	int flags;
	void *ptr;
	LinkedList_DeclareRef(ref(Event));
};

LinkedList_DeclareList(ref(Event), ref(Events));

class {
	ref(Events) events;
};

rsdef(self, New);
def(void, Destroy);
def(void, Enqueue, void *ptr, int flags);
def(bool, HasEvents);
def(ref(Event), Pop);
def(void, Prune, void *ptr);

#undef self
