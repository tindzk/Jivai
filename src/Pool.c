#import "String.h"
#import "Exception.h"
#import "Pool.h"

#define self Pool

Singleton(self);

rsdef(self, New) {
	return (self) {
		.sess = NULL,
		.bundling = ref(Bundling_Disabled)
	};
}

overload rdef(ref(Session) *, CreateSession, __unused RdString name, ref(Session) *parent) {
	ref(Session) *sess = Memory_Alloc(sizeof(ref(Session)));

	sess->hasParent = false;
	sess->prev  = NULL;
	sess->next  = NULL;
	sess->child = NULL;
	sess->alloc = NULL;

	if (parent != NULL) {
		if (parent->child == NULL) {
			parent->child = sess;

			sess->prev      = parent;
			sess->hasParent = true;

			return sess;
		}

		/* Follow trail and insert as last element. */
		for (ref(Session) *cur = parent; true; cur = cur->next) {
			if (cur->next == NULL) {
				cur->next = sess;
				sess->prev = cur;
				break;
			}
		}
	}

	return sess;
}

overload rdef(ref(Session) *, CreateSession, RdString name) {
	return call(CreateSession, name, this->sess);
}

static def(size_t, _Free, ref(Allocation) *alloc);

rdef(size_t, Dispose, ref(Session) *sess) {
	size_t size = 0;

	if (sess->alloc != NULL) {
		for (ref(Allocation) *cur = sess->alloc; cur != NULL; ) {
			ref(Allocation) *next = cur->next;
			size += call(_Free, cur);
			cur = next;
		}
	}

	while (sess->child != NULL) {
		ref(Session) *next = sess->child->next;
		size += call(Dispose, sess->child);
		sess->child = next;
	}

	if (this->sess == sess) {
		if (this->sess->hasParent) {
			this->sess = this->sess->prev;
		} else {
			this->sess = NULL;
		}
	}

	if (this->sessDefault == sess) {
		this->sessDefault = NULL;
	}

	if (sess->prev != NULL) {
		if (sess->hasParent) {
			sess->prev->child = sess->next;

			if (sess->next != NULL) {
				sess->next->hasParent = true;
			}
		} else {
			sess->prev->next = sess->next;
		}
	}

	if (sess->next != NULL) {
		sess->next->prev = sess->prev;
	}

	Memory_Free(sess);

	return size;
}

overload rdef(ref(Session) *, SetSession, ref(Session) *sess, bool isDefault) {
	ref(Session) *old = this->sess;
	this->sess = sess;

	if (isDefault) {
		this->sessDefault = sess;
	}

	return old;
}

overload rdef(ref(Session) *, SetSession, ref(Session) *sess) {
	return call(SetSession, sess, false);
}

rdef(ref(Session) *, GetDefaultSession) {
	return this->sessDefault;
}

static def(void, _Link, ref(Allocation) *alloc, ref(Allocation) *parent) {
	if (alloc->hasParent) {
		throw(HasParent);
	}

	if (parent->child == NULL) {
		parent->child = alloc;

		alloc->prev      = parent;
		alloc->hasParent = true;

		return;
	}

	for (ref(Allocation) *cur = parent->child; true; cur = cur->next) {
		if (cur->next == NULL) {
			cur->next = alloc;
			alloc->prev = cur;
			break;
		}
	}
}

def(void, Link, void *alloc, void *parent) {
	ref(Allocation) *_alloc  = alloc  - sizeof(ref(Allocation));
	ref(Allocation) *_parent = parent - sizeof(ref(Allocation));

	call(_Link, _alloc, _parent);
}

__malloc rdef(void *, Alloc, size_t size) {
	ref(Allocation) *alloc = Memory_Alloc(sizeof(ref(Allocation)) + size);

	alloc->size  = size;
	alloc->sess  = this->sess;
	alloc->prev  = NULL;
	alloc->next  = NULL;
	alloc->child = NULL;
	alloc->hasParent = false;

	if (alloc->sess != NULL) {
		if (alloc->sess->alloc == NULL) {
			alloc->sess->alloc = alloc;
		} else {
			for (ref(Allocation) *cur = alloc->sess->alloc; true; cur = cur->next) {
				if (cur->next == NULL) {
					cur->next = alloc;
					alloc->prev = cur;
					break;
				}
			}
		}
	}

	if (this->bundling == ref(Bundling_AwaitingAlloc)) {
		this->bundling  = ref(Bundling_Ok);
		this->baseAlloc = alloc;
	} else if (this->bundling == ref(Bundling_Ok)) {
		call(_Link, alloc, this->baseAlloc);
	}

	return alloc->buf;
}

__malloc rdef(void *, Realloc, void *addr, size_t size) {
	ref(Allocation) *old = addr - sizeof(ref(Allocation));

	ref(Allocation) *parent = NULL;

	if (old->hasParent) {
		parent = old->prev;
	}

	ref(Allocation) *alloc =
		Memory_Realloc(old, sizeof(ref(Allocation)) + size);

	alloc->size = size;

	if (parent != NULL) {
		parent->child = alloc;
	}

	if (alloc->prev != NULL) {
		alloc->prev->next = alloc;
	}

	if (alloc->next != NULL) {
		alloc->next->prev = alloc;
	}

	if (alloc->sess != NULL) {
		if (alloc->sess->alloc == old) {
			alloc->sess->alloc = alloc;
		}
	}

	return alloc->buf;
}

static def(void, Unlink, ref(Allocation) *alloc) {
	if (alloc->sess != NULL) {
		if (alloc->sess->alloc == alloc) {
			if (alloc->hasParent) {
				alloc->sess->alloc = alloc->next;
			} else {
				alloc->sess->alloc =
					(alloc->prev != NULL)
						? alloc->prev
						: alloc->next;
			}
		}
	}

	if (alloc->prev != NULL) {
		if (alloc->hasParent) {
			alloc->prev->child = alloc->next;

			if (alloc->next != NULL) {
				alloc->next->hasParent = true;
			}
		} else {
			alloc->prev->next = alloc->next;
		}
	}

	if (alloc->next != NULL) {
		alloc->next->prev = alloc->prev;
	}
}

static def(size_t, _Free, ref(Allocation) *alloc) {
	if (this->baseAlloc == alloc) {
		this->bundling  = ref(Bundling_AwaitingAlloc);
		this->baseAlloc = NULL;
	}

	call(Unlink, alloc);

	size_t size = alloc->size;

	while (alloc->child != NULL) {
		ref(Allocation) *next = alloc->child->next;
		size += call(_Free, alloc->child);
		alloc->child = next;
	}

	Memory_Free(alloc);

	return size;
}

def(size_t, Free, void *addr) {
	ref(Allocation) *alloc = addr - sizeof(ref(Allocation));
	return call(_Free, alloc);
}

rdef(size_t, GetSize, void *addr) {
	ref(Allocation) *alloc = addr - sizeof(ref(Allocation));
	return alloc->size;
}

__malloc rdef(void *, Clone, void *addr) {
	ref(Allocation) *alloc = addr - sizeof(ref(Allocation));
	return call(Alloc, alloc->size);
}

def(void, Bundle, __unused RdString name) {
	if (this->bundling != ref(Bundling_Disabled)) {
		throw(AlreadyBundling);
	}

	this->bundling = ref(Bundling_AwaitingAlloc);
}

def(void, Commit) {
	if (this->bundling == ref(Bundling_Disabled)) {
		throw(NotBundling);
	}

	this->bundling  = ref(Bundling_Disabled);
	this->baseAlloc = NULL;
}
