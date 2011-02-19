#import "Types.h"

#define self Arena

// @exc NotAllocated
// @exc LimitExceeded

#ifndef Arena_Allocations
#define Arena_Allocations 10000
#endif

record(ArenaSegment) {
	void *addr;
	size_t size;
};

class {
	size_t upper;
	bool fragmented;
	ssize_t free;
	ArenaSegment allocs[Arena_Allocations];
};

def(void, Init);
def(void *, Alloc, size_t size);
def(void *, Realloc, void *addr, size_t newSize);
def(size_t, GetSize, void *addr);
def(void, Free, void *addr);

SingletonPrototype(self);

#undef self
