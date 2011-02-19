#import "Arena.h"
#import "Memory.h"
#import "Exception.h"

#define self Arena

Singleton(self);

def(void, Init) {
	this->upper = 0;
	this->fragmented = false;
	this->free = -1;
}

static def(ArenaSegment *, GetSegment, void *addr) {
	if (addr == NULL) {
		return NULL;
	}

	forward (i, this->upper) {
		if (addr >= this->allocs[i].addr &&
			addr <= this->allocs[i].addr + this->allocs[i].size)
		{
			return &this->allocs[i];
		}
	}

	return NULL;
}

def(bool, Contains, void *addr) {
	return call(GetSegment, addr) != NULL;
}

def(void *, Alloc, size_t size) {
	ArenaSegment data = {
		.addr = Memory_Alloc(size),
		.size = size
	};

	if (this->free == -1) {
		if (this->fragmented) {
			reverse (i, this->upper) {
				if (this->allocs[i].addr == NULL) {
					this->free = i;
					break;
				}
			}

			/* No free slot found in 0..this->upper-1. */
			if (this->free == -1) {
				this->fragmented = false;
			}
		}
	}

	if (this->free != -1) {
		this->allocs[this->free] = data;
		this->free = -1;

		return data.addr;
	}

	if (this->upper < nElems(this->allocs)) {
		this->allocs[this->upper] = data;
		this->upper++;

		return data.addr;
	}

	throw(LimitExceeded);

	return NULL;
}

/* Respects the old offset. */
def(void *, Realloc, void *addr, size_t newSize) {
	ArenaSegment *item = call(GetSegment, addr);

	if (item == NULL) {
		return NULL;
	}

	size_t ofs = addr - item->addr;

	item->addr = Memory_Realloc(item->addr, newSize);
	item->size = newSize;

	return item->addr + ofs;
}

def(size_t, GetOffset, void *addr) {
	ArenaSegment *item = call(GetSegment, addr);

	if (item == NULL) {
		throw(NotAllocated);
	}

	return addr - item->addr;
}

def(size_t, GetSize, void *addr) {
	ArenaSegment *item = call(GetSegment, addr);

	if (item == NULL) {
		throw(NotAllocated);
	}

	size_t ofs = addr - item->addr;

	return item->size - ofs;
}

def(void, Free, void *addr) {
	ArenaSegment *item = call(GetSegment, addr);

	if (item == NULL) {
		throw(NotAllocated);
	}

	Memory_Free(item->addr);

	item->addr = NULL;

	if (this->free == -1) {
		this->free = item - this->allocs;
	} else {
		this->fragmented = true;
	}
}
