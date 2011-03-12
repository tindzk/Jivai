#import "Types.h"

#define self Pool

// @exc HasParent
// @exc NotBundling
// @exc AlreadyBundling

struct ref(Session);

record(ref(Allocation)) {
	bool hasParent;
	ref(Allocation)     *prev;  /* If `hasParent' is true, this contains the parent. */
	ref(Allocation)     *next;  /* Next allocation.          */
	ref(Allocation)     *child; /* Child allocation.         */
	struct ref(Session) *sess;  /* Session points here.      */
	size_t              size;   /* Size of allocated buffer. */
	char                buf[];  /* Buffer.                   */
};

record(ref(Session)) {
	bool hasParent;
	ref(Session)    *prev;  /* Previous or parent session. */
	ref(Session)    *next;  /* Next session.               */
	ref(Session)    *child; /* Child session.              */
	ref(Allocation) *alloc; /* First allocation.           */
};

set(ref(Bundling)) {
	ref(Bundling_Disabled),
	ref(Bundling_AwaitingAlloc),
	ref(Bundling_Ok)
};

class {
	ref(Session)    *sess;      /* Current session.    */
	ref(Allocation) *baseAlloc; /* Current allocation. */
	ref(Bundling)   bundling;   /* Bundling mode.      */
};

rsdef(self, New);
rdef(ref(Session) *, CreateSession, __unused ProtString name, ref(Session) *parent);
rdef(size_t, Dispose, ref(Session) *sess);
rdef(ref(Session) *, SetSession, ref(Session) *sess);
def(void, Link, void *alloc, void *parent);
__malloc rdef(void *, Alloc, size_t size);
__malloc rdef(void *, Realloc, void *addr, size_t size);
def(size_t, Free, void *addr);
rdef(size_t, GetSize, void *addr);
__malloc rdef(void *, Clone, void *addr);
def(void, Bundle, ProtString name);
def(void, Commit);

SingletonPrototype(self);

#undef self
