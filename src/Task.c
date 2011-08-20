#import "Task.h"

#define self Task

rsdef(self *, new, void *data, void (*destroy)(Instance $this)) {
	self *res = Memory_New(sizeof(self));

	res->next    = null;
	res->data    = data;
	res->destroy = destroy;

	return res;
}

def(void, destroy) {
	if (this->destroy != null) {
		this->destroy(this->data);
	}

	Memory_Destroy(this);
}

#undef self

#define self Tasks

rsdef(self, new) {
	return (self) {
		.first = null
	};
}

def(void, destroy) {
	Task *next = null;

	for (Task *cur = this->first; cur != null; cur = next) {
		next = cur->next;
		Task_destroy(cur);
	}
}

def(void, enqueue, Task *task) {
	if (this->first == null) {
		this->first = task;
		return;
	}

	for (Task *cur = this->first; cur != null; cur = cur->next) {
		if (cur->next == null) {
			cur->next = task;
			break;
		}
	}
}
