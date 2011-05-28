#import <LinkedMemory.h>

#import "TestSuite.h"

#define self tsLinkedMemory

class {

};

tsRegister("LinkedMemory") {
	return true;
}

tsCase(Acute, "Allocating") {
	void *ptr = LinkedMemory_Alloc(LinkedMemory_GetInstance(), 8);
	LinkedMemory_Free(LinkedMemory_GetInstance(), ptr);
}

tsCase(Acute, "Linking") {
	void *master = LinkedMemory_Alloc(LinkedMemory_GetInstance(), 32);

	void *slave  = LinkedMemory_Alloc(LinkedMemory_GetInstance(), 8);
	void *slave2 = LinkedMemory_Alloc(LinkedMemory_GetInstance(), 8);
	void *slave3 = LinkedMemory_Alloc(LinkedMemory_GetInstance(), 8);

	Assert($("Size"),
		LinkedMemory_GetSize(LinkedMemory_GetInstance(), slave)  >= 8 &&
		LinkedMemory_GetSize(LinkedMemory_GetInstance(), slave2) >= 8 &&
		LinkedMemory_GetSize(LinkedMemory_GetInstance(), slave3) >= 8);

	LinkedMemory_Link(LinkedMemory_GetInstance(), slave,  master);
	LinkedMemory_Link(LinkedMemory_GetInstance(), slave2, master);
	LinkedMemory_Link(LinkedMemory_GetInstance(), slave3, master);

	/* Even though you linked the allocations, you can still free the slaves
	 * manually.
	 */
	LinkedMemory_Free(LinkedMemory_GetInstance(), slave);
	LinkedMemory_Free(LinkedMemory_GetInstance(), slave2);
	LinkedMemory_Free(LinkedMemory_GetInstance(), slave3);

	size_t size = LinkedMemory_Free(LinkedMemory_GetInstance(), master);

	Assert($("Free"), size >= 32);
}

tsCase(Acute, "Linking") {
	void *master = LinkedMemory_Alloc(LinkedMemory_GetInstance(), 32);

	void *slave  = LinkedMemory_Alloc(LinkedMemory_GetInstance(), 8);
	void *slave2 = LinkedMemory_Alloc(LinkedMemory_GetInstance(), 8);
	void *slave3 = LinkedMemory_Alloc(LinkedMemory_GetInstance(), 8);

	LinkedMemory_Link(LinkedMemory_GetInstance(), slave,  master);
	LinkedMemory_Link(LinkedMemory_GetInstance(), slave2, master);
	LinkedMemory_Link(LinkedMemory_GetInstance(), slave3, master);

	size_t size = LinkedMemory_Free(LinkedMemory_GetInstance(), master);

	Assert($("Free"), size >= 32 + 3 * 8);
}

tsCase(Acute, "Linking") {
	void *master = LinkedMemory_Alloc(LinkedMemory_GetInstance(), 32);

	void *slave  = LinkedMemory_Alloc(LinkedMemory_GetInstance(), 8);
	void *slave2 = LinkedMemory_Alloc(LinkedMemory_GetInstance(), 8);
	void *slave3 = LinkedMemory_Alloc(LinkedMemory_GetInstance(), 8);

	/* Note that only one parent can be set. */
	LinkedMemory_Link(LinkedMemory_GetInstance(), slave,  master);
	LinkedMemory_Link(LinkedMemory_GetInstance(), slave2, master);
	LinkedMemory_Link(LinkedMemory_GetInstance(), slave3, master);

	/* Reallocate the first slave in order to verify that it's also updated in
	 * the parent allocation.
	 */
	(void) LinkedMemory_Realloc(LinkedMemory_GetInstance(), slave, 16);

	size_t size = LinkedMemory_Free(LinkedMemory_GetInstance(), master);

	Assert($("Free"), size >= 32 + 16 + 2 * 8);
}

tsCase(Acute, "Bundling") {
	LinkedMemory_Bundle(LinkedMemory_GetInstance(), $("Bundle"));

	void *temp = LinkedMemory_Alloc(LinkedMemory_GetInstance(), 8);
	LinkedMemory_Free(LinkedMemory_GetInstance(), temp);

	/* As the previous allocation is already destroyed, this will be the
	 * master.
	 */
	void *master = LinkedMemory_Alloc(LinkedMemory_GetInstance(), 32);

	(void) LinkedMemory_Alloc(LinkedMemory_GetInstance(), 8);
	(void) LinkedMemory_Alloc(LinkedMemory_GetInstance(), 8);
	(void) LinkedMemory_Alloc(LinkedMemory_GetInstance(), 8);

	temp = LinkedMemory_Alloc(LinkedMemory_GetInstance(), 8);
	LinkedMemory_Free(LinkedMemory_GetInstance(), temp);

	LinkedMemory_Commit(LinkedMemory_GetInstance());

	size_t size = LinkedMemory_Free(LinkedMemory_GetInstance(), master);

	Assert($("Free"), size >= 32 + 3 * 8);
}

tsCase(Acute, "Sessions") {
	LinkedMemory_Session *sess = LinkedMemory_CreateSession(LinkedMemory_GetInstance(), $(""), NULL);

	(void) LinkedMemory_SetSession(LinkedMemory_GetInstance(), sess);

	void *tmp = LinkedMemory_Alloc(LinkedMemory_GetInstance(), 8);
	LinkedMemory_Free(LinkedMemory_GetInstance(), tmp);

	(void) LinkedMemory_Alloc(LinkedMemory_GetInstance(), 8);
	(void) LinkedMemory_Alloc(LinkedMemory_GetInstance(), 8);
	(void) LinkedMemory_Alloc(LinkedMemory_GetInstance(), 8);

	size_t size = LinkedMemory_Dispose(LinkedMemory_GetInstance(), sess);

	Assert($("Free"), size >= 3 * 8);
}

tsCase(Acute, "Nested Sessions") {
	LinkedMemory_Session *root  = LinkedMemory_CreateSession(LinkedMemory_GetInstance(), $("Root"),  NULL);
	LinkedMemory_Session *child = LinkedMemory_CreateSession(LinkedMemory_GetInstance(), $("Child"), root);

	(void) LinkedMemory_SetSession(LinkedMemory_GetInstance(), root);

	(void) LinkedMemory_Alloc(LinkedMemory_GetInstance(), 2);
	(void) LinkedMemory_Alloc(LinkedMemory_GetInstance(), 4);

	LinkedMemory_Session *result = LinkedMemory_SetSession(LinkedMemory_GetInstance(), child);
	Assert($("Parent"), root == result);

	(void) LinkedMemory_Alloc(LinkedMemory_GetInstance(), 6);
	(void) LinkedMemory_Alloc(LinkedMemory_GetInstance(), 8);

	size_t size = LinkedMemory_Dispose(LinkedMemory_GetInstance(), root);

	Assert($("Free"), size >= 2 + 4 + 6 + 8);
}

tsCase(Acute, "Cloning") {
	char *orig = LinkedMemory_Alloc(LinkedMemory_GetInstance(), 10);
	Memory_Copy(orig, "Hello", 5);

	char *copy = LinkedMemory_Clone(LinkedMemory_GetInstance(), orig);
	Assert($("Equals"), Memory_Equals(orig, copy, 5));

	LinkedMemory_Free(LinkedMemory_GetInstance(), copy);
	LinkedMemory_Free(LinkedMemory_GetInstance(), orig);
}
