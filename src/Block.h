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

#import "Memory.h"

enum {
	Block_Flags_RefCountMask   = (0xffff),
	Block_Flags_IsHeap         = (1 << 24),
	Block_Flags_HasCopyDispose = (1 << 25),
	Block_Flags_IsGlobal       = (1 << 28),
	Block_Flags_FieldIsByRef   = 8
} Block_Flags;

typedef struct {
	void *isa;
	int flags;
	int reserved;
	void (*invoke)(void *, ...);
	struct {
		unsigned long int reserved;
		unsigned long int size;
		void (*copy)(void *dst, void *src);
		void (*dispose)(void *);
	} *descriptor;
} Block_Layout;

typedef struct _Block_ByRef {
	void *isa;
	struct _Block_ByRef *forwarding;
	int flags;
	size_t size;
	void (*keep)(struct _Block_ByRef *dst, struct _Block_ByRef *src);
	void (*destroy)(struct _Block_ByRef *);
	/* long shared[0]; */
} Block_ByRef;

typedef struct {
	void *isa;
	struct Block_ByRef *forwarding;
	int flags;
	int size;
} Block_ByRefHeader;

void *_NSConcreteStackBlock[32];
void *_NSConcreteMallocBlock[32];
void *_NSConcreteGlobalBlock[32];

void* _Block_Copy(Block_Layout *block);
void _Block_Release(Block_Layout *block);
void _Block_object_assign(void *dest, void const *src, int const flags);
void _Block_object_dispose(const void *object, const int flags);

#define Block_Copy(...) \
	((__typeof(__VA_ARGS__)) _Block_Copy((Block_Layout *)(__VA_ARGS__)))

#define Block_Release(block) \
	_Block_Release((Block_Layout *)(block))
