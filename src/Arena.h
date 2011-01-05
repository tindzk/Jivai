#import "Types.h"

#define self Arena

// @exc NotAllocated
// @exc LimitExceeded

#ifndef Arena_Allocations
#define Arena_Allocations 1024
#endif

record(ArenaSegment) {
	void *addr;
	size_t size;

	bool resizable;
};

class {
	size_t upper;
	bool fragmented;
	ssize_t free;
	ArenaSegment allocs[Arena_Allocations];
};

def(void, Init);
def(void, AddItem, void *addr, size_t size, bool resizable);
def(void *, Alloc, size_t size);
def(ArenaSegment *, GetSegment, void *ptr);
def(bool, Contains, void *addr);
def(void *, AddBuffer, void *addr, size_t size);
def(void *, Realloc, void *addr, size_t newSize);
def(ssize_t, GetOffset, void *addr);
def(ssize_t, GetSize, void *addr);
def(void, Free, void *addr);

SingletonPrototype(self);

#undef self
