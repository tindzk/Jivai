#import "String.h"
#import "Pool.h"

#define self Pool

Singleton(self);

rsdef(self, New) {
	return (self) {
		.sess = NULL,
		.last = NULL,
		.bundling = ref(Bundling_Disabled)
	};
}

rdef(ref(Session) *, CreateSession, __unused ProtString name) {
	ref(Session) *sess = Memory_Alloc(sizeof(ref(Session)));

	sess->alloc     = NULL;
	sess->prevStack = NULL;
	sess->prevCreat = this->last;

	this->last = sess;

	return sess;
}

rdef(size_t, Dispose, ref(Session) *sess) {
	size_t size = 0;

	for (ref(Allocation) *cur = sess->alloc; cur != NULL; ) {
		ref(Allocation) *free = cur;

		size += cur->size;
		cur   = cur->prev;

		Memory_Free(free);
	}

	if (this->last == sess) {
		this->last = this->last->prevCreat;
	} else {
		for (ref(Session) *cur = this->last; cur != NULL; cur = cur->prevCreat) {
			if (cur->prevCreat == sess) {
				cur->prevCreat = cur->prevCreat->prevCreat;
				break;
			}
		}
	}

	Memory_Free(sess);

	return size;
}

def(void, Push, ref(Session) *sess, __unused ProtString name) {
	sess->prevStack = this->sess;
	this->sess = sess;
}

def(void, Pop, ref(Session) *sess) {
	if (this->sess != sess) {
		throw(StackCorruption);
	}

	this->sess = sess->prevStack;
	sess->prevStack = NULL;
}

__malloc rdef(void *, Alloc, size_t size) {
	ref(Allocation) *alloc = Memory_Alloc(sizeof(ref(Allocation)) + size);

	alloc->size = size;
	alloc->prev = NULL;
	alloc->next = NULL;
	alloc->freeNext = NULL;

	if (this->sess != NULL) {
		if (this->sess->alloc == NULL) {
			this->sess->alloc = alloc;
		} else {
			/* Follow trail and insert as last element. */
			for (ref(Allocation) *cur = this->sess->alloc; true; cur = cur->next) {
				if (cur->next == NULL) {
					cur->next = alloc;

					if (this->bundling == ref(Bundling_Ok)) {
						cur->freeNext = alloc;
					}

					alloc->prev = cur;
					break;
				}
			}
		}
	}

	if (this->bundling == ref(Bundling_AwaitingAlloc)) {
		this->bundling = ref(Bundling_Ok);
	}

	return alloc->buf;
}

__malloc rdef(void *, Realloc, void *addr, size_t size) {
	ref(Allocation) *old   = addr - sizeof(ref(Allocation));
	ref(Allocation) *alloc = Memory_Realloc(old, sizeof(ref(Allocation)) + size);

	alloc->size = size;

	if (alloc->prev != NULL) {
		alloc->prev->next = alloc;
	}

	if (alloc->next != NULL) {
		alloc->next->prev = alloc;
	}

	for (ref(Session) *cur = this->last; cur != NULL; cur = cur->prevCreat) {
		if (cur->alloc == old) {
			cur->alloc = alloc;
			break;
		}
	}

	return alloc->buf;
}

static def(void, Unlink, ref(Allocation) *alloc) {
	for (ref(Session) *cur = this->last; cur != NULL; cur = cur->prevCreat) {
		if (cur->alloc == alloc) {
			cur->alloc =
				(alloc->prev != NULL)
					? alloc->prev
					: alloc->next;

			break;
		}
	}

	if (alloc->prev != NULL) {
		alloc->prev->next     = alloc->next;
		alloc->prev->freeNext = alloc->freeNext;
	}

	if (alloc->next != NULL) {
		alloc->next->prev = alloc->prev;
	}
}

def(size_t, Free, void *addr) {
	ref(Allocation) *alloc = addr - sizeof(ref(Allocation));

	call(Unlink, alloc);

	size_t size = alloc->size;

	/* Only free the subsequent chunks when we are dealing with the first
	 * bundled chunk.
	 */
	if (alloc->prev == NULL || alloc->prev->freeNext == NULL) {
		for (ref(Allocation) *cur = alloc->freeNext; cur != NULL; ) {
			ref(Allocation) *tmp = cur;

			call(Unlink, cur);

			size += cur->size;
			cur   = cur->freeNext;

			Memory_Free(tmp);
		}
	}

	Memory_Free(alloc);

	return size;
}

rdef(size_t, GetSize, void *addr) {
	ref(Allocation) *alloc = addr - sizeof(ref(Allocation));
	return alloc->size;
}

__malloc rdef(void *, Clone, void *addr) {
	ref(Allocation) *alloc = addr - sizeof(ref(Allocation));
	return call(Alloc, alloc->size);
}

def(void, Bundle, __unused ProtString name) {
	if (this->sess == NULL) {
		throw(NoSession);
	}

	this->bundling = ref(Bundling_AwaitingAlloc);
}

def(void, Commit) {
	this->bundling = ref(Bundling_Disabled);
}
