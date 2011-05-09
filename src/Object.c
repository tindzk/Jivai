#import "Object.h"
#import "Exception.h"

#define self DynObject

rsdef(DynObject, New, size_t size) {
	if (size == 0) {
		return (DynObject) { .addr = NULL };
	}

	return (DynObject) {
		.addr = Pool_Alloc(Pool_GetInstance(), size)
	};
}

overload sdef(void, Destroy, DynObject $this) {
	if (($this).addr != NULL) {
		Pool_Free(Pool_GetInstance(), ($this).addr);
	}
}

overload sdef(void, Destroy, void *addr) {
	if (addr != NULL) {
		Pool_Free(Pool_GetInstance(), addr);
	}
}

rsdef(void *, GetMember, DynObject $this, size_t ofs) {
	assert($this.addr != NULL);
	assert(ofs <= Pool_GetSize(Pool_GetInstance(), $this.addr));

	return $this.addr + ofs;
}

rsdef(bool, IsValid, DynObject $this) {
	return $this.addr != NULL;
}
