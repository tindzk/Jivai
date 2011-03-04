#import "Types.h"

#define self Pool

// @exc NoSession
// @exc StackCorruption

record(ref(Allocation)) {
	ref(Allocation) *prev;
	ref(Allocation) *next;
	ref(Allocation) *freeNext;
	size_t          size;
	char            buf[];
};

record(ref(Session)) {
	ref(Allocation) *alloc;     /* First allocation.     */
	ref(Session)    *prevStack; /* Previous session.     */
	ref(Session)    *prevCreat; /* Last created session. */
};

set(ref(Bundling)) {
	ref(Bundling_Disabled),
	ref(Bundling_AwaitingAlloc),
	ref(Bundling_Ok)
};

class {
	ref(Session)  *sess;    /* Current session.    */
	ref(Session)  *last;    /* Last added session. */
	ref(Bundling) bundling; /* Bundling mode.      */
};

rsdef(self, New);
rdef(ref(Session) *, CreateSession, ProtString name);
rdef(size_t, Dispose, ref(Session) *sess);
def(void, Push, ref(Session) *sess, ProtString name);
def(void, Pop, ref(Session) *sess);
__malloc rdef(void *, Alloc, size_t size);
__malloc rdef(void *, Realloc, void *addr, size_t size);
def(size_t, Free, void *addr);
rdef(size_t, GetSize, void *addr);
__malloc rdef(void *, Clone, void *addr);
def(void, Bundle, ProtString name);
def(void, Commit);

SingletonPrototype(self);

#undef self
