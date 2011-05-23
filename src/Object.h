#import "OO.h"

#define self DynObject

/* References an heap-allocated object. */
variant(self) {
	Instance inst;
	struct {
		void *addr;
	};
} transparentUnion;

rsdef(self, New, size_t size);
overload sdef(void, Destroy, self $this);
overload sdef(void, Destroy, void *addr);
rsdef(void *, GetMember, self $this, size_t ofs);
rsdef(bool, IsValid, self $this);

#define DynObject_Call(method, object, ...)    \
	if (method != NULL) {                      \
		method((object).inst, ## __VA_ARGS__); \
	}

#undef self
