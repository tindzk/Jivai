#import "Logger.h"

#define self Task

class {
	self *next;
	void (*destroy)(Instance $this);
	void *data;
};

rsdef(self *, new, void *data, void (*destroy)(Instance $this));
def(void, destroy);

#undef self

#define self Tasks

class {
	Task *first;
};

rsdef(self, new);
def(void, destroy);
def(void, enqueue, Task *task);

#undef self
