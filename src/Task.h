#import "Logger.h"

#define self Task

class {
	self *next;
	void (*destroy)(GenericInstance $this);
	char data[];
};

rsdef(self *, New, size_t size, void (*destroy)(GenericInstance $this));
def(void, Destroy);

#undef self

#define self Tasks

class {
	Task *first;
};

rsdef(self, New);
def(void, Destroy);
def(void, Enqueue, Task *task);

#undef self
