#import "Object.h"
#import "Exception.h"

#define self DynObject

rsdef(DynObject, New, size_t size) {
	if (size == 0) {
		return (DynObject) { .addr = NULL };
	}

	return (DynObject) {
		.addr = Memory_New(size)
	};
}

overload sdef(void, Destroy, DynObject $this) {
	if (($this).addr != NULL) {
		Memory_Destroy($this.addr);
	}
}

overload sdef(void, Destroy, void *addr) {
	if (addr != NULL) {
		Memory_Destroy(addr);
	}
}

rsdef(void *, GetMember, DynObject $this, size_t ofs) {
	assert($this.addr != NULL);
	assert(ofs <= Memory_GetSize($this.addr));

	return $this.addr + ofs;
}

rsdef(bool, IsValid, DynObject $this) {
	return $this.addr != NULL;
}
