/*
 * Copyright 2008-2010 Apple, Inc. Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "Block.h"

void *_NSConcreteStackBlock[32]  = { 0 };
void *_NSConcreteMallocBlock[32] = { 0 };
void *_NSConcreteGlobalBlock[32] = { 0 };

static __inline bool Block_AtomicCompareAndSwapLong(long oldl, long newl, long volatile *dst) {
	return __sync_bool_compare_and_swap(dst, oldl, newl);
}

static __inline bool Block_AtomicCompareAndSwapInt(int oldi, int newi, int volatile *dst) {
	return __sync_bool_compare_and_swap(dst, oldi, newi);
}

static int Block_LatchingIncrInt(int *where) {
	while (true) {
		int old = *(volatile int *) where;

		if ((old & Block_Flags_RefCountMask) == Block_Flags_RefCountMask) {
			return Block_Flags_RefCountMask;
		}

		if (Block_AtomicCompareAndSwapInt(old, old + 1, (volatile int *)where)) {
			return old + 1;
		}
	}
}

static int Block_LatchingDecrInt(int *where) {
	while (true) {
		int old = *(volatile int *) where;

		if ((old & Block_Flags_RefCountMask) == Block_Flags_RefCountMask) {
			return Block_Flags_RefCountMask;
		}

		if ((old & Block_Flags_RefCountMask) == 0) {
			return 0;
		}

		if (Block_AtomicCompareAndSwapInt(old, old - 1, (volatile int *)where)) {
			return old - 1;
		}
	}
}

void* _Block_Copy(Block_Layout *block) {
	Block_Layout *result = block;

	if (block->flags & Block_Flags_IsHeap) {
		/* Heap block. */
		Block_LatchingIncrInt(&block->flags);
	} else if (block->flags & Block_Flags_IsGlobal) {
		/* Global block. */
	} else {
		/* Stack block. */
		result = Memory_Alloc(block->descriptor->size);

		Memory_Move(result, block, block->descriptor->size);

		result->flags |= Block_Flags_IsHeap;
		result->isa = _NSConcreteMallocBlock;

		if (result->flags & Block_Flags_HasCopyDispose) {
			(*block->descriptor->copy)(result, block);
		}
	}

	return result;
}

void _Block_Release(Block_Layout *block) {
	int32_t refCount = Block_LatchingDecrInt(&block->flags) & Block_Flags_RefCountMask;

	if (block->flags & Block_Flags_IsHeap) {
		if (refCount == 0) {
			if (block->flags & Block_Flags_HasCopyDispose) {
				(*block->descriptor->dispose)(block);
			}

			Memory_Free(block);
		}
	}
}

void _Block_object_assign(Block_ByRef **dest, Block_ByRef *src, const int flags) {
	if (flags & Block_Flags_FieldIsByRef) {
		Block_ByRef *copy = (Block_ByRef *) Memory_Alloc(src->size);

		copy->flags = src->flags | Block_Flags_IsHeap | 2;
		copy->forwarding = copy; /* Patch heap copy to point to itself (skip write-barrier). */
		src->forwarding  = copy; /* Patch stack to point to heap copy. */
		copy->size = src->size;

		Memory_Move(&copy->keep, &src->keep,
			(size_t)(src->size - sizeof(Block_ByRefHeader)));

		*dest = src->forwarding;
	}
}

void _Block_object_dispose(Block_Layout *object, const int flags) {
	if (flags & Block_Flags_FieldIsByRef) {
		Block_ByRef *shared = ((Block_ByRef *) object)->forwarding;

		if ((shared->flags & Block_Flags_RefCountMask) > 0) {
			if ((Block_LatchingDecrInt(&shared->flags) & Block_Flags_RefCountMask) == 0) {
				Memory_Free(shared);
			}
		}
	}
}
