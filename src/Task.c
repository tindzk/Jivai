#import "Task.h"

#define self Task

rsdef(self *, New, size_t size, void (*destroy)(GenericInstance $this)) {
	self *res = Pool_Alloc(Pool_GetInstance(), sizeof(self) + size);

	res->next    = NULL;
	res->destroy = destroy;

	return res;
}

def(void, Destroy) {
	if (this->destroy != NULL) {
		this->destroy(this->data);
	}

	Pool_Free(Pool_GetInstance(), this);
}

#undef self

#define self Tasks

rsdef(self, New) {
	return (self) {
		.first = NULL
	};
}

def(void, Destroy) {
	Task *next = NULL;

	for (Task *cur = this->first; cur != NULL; cur = next) {
		next = cur->next;
		Task_Destroy(cur);
	}
}

def(void, Enqueue, Task *task) {
	if (this->first == NULL) {
		this->first = task;
		return;
	}

	for (Task *cur = this->first; cur != NULL; cur = cur->next) {
		if (cur->next == NULL) {
			cur->next = task;
			break;
		}
	}
}
