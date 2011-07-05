#import <sys/mman.h>

#import "String.h"

#define self MemoryMappedFile

exc(InvalidFile)
exc(UnknownError)

class {
	u64 size;
	void *addr;
};

rsdef(self, new, RdString path);
def(void, destroy);

static alwaysInline def(u64, getSize) {
	return this->size;
}

static alwaysInline def(void *, getAddress) {
	return this->addr;
}

#undef self
