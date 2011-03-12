#import <String.h>
#import <Pool.h>

#import "TestSuite.h"

#define self tsPool

class {

};

tsRegister("Pool") {
	return true;
}

tsCase(Acute, "Allocating") {
	void *ptr = Pool_Alloc(Pool_GetInstance(), 8);
	Pool_Free(Pool_GetInstance(), ptr);
}

tsCase(Acute, "Linking") {
	void *master = Pool_Alloc(Pool_GetInstance(), 32);

	void *slave  = Pool_Alloc(Pool_GetInstance(), 8);
	void *slave2 = Pool_Alloc(Pool_GetInstance(), 8);
	void *slave3 = Pool_Alloc(Pool_GetInstance(), 8);

	Assert($("Size"),
		Pool_GetSize(Pool_GetInstance(), slave)  == 8 &&
		Pool_GetSize(Pool_GetInstance(), slave2) == 8 &&
		Pool_GetSize(Pool_GetInstance(), slave3) == 8);

	Pool_Link(Pool_GetInstance(), slave,  master);
	Pool_Link(Pool_GetInstance(), slave2, master);
	Pool_Link(Pool_GetInstance(), slave3, master);

	/* Even though you linked the allocations, you can still free the slaves
	 * manually.
	 */
	Pool_Free(Pool_GetInstance(), slave);
	Pool_Free(Pool_GetInstance(), slave2);
	Pool_Free(Pool_GetInstance(), slave3);

	size_t size = Pool_Free(Pool_GetInstance(), master);

	Assert($("Free"), size == 32);
}

tsCase(Acute, "Linking") {
	void *master = Pool_Alloc(Pool_GetInstance(), 32);

	void *slave  = Pool_Alloc(Pool_GetInstance(), 8);
	void *slave2 = Pool_Alloc(Pool_GetInstance(), 8);
	void *slave3 = Pool_Alloc(Pool_GetInstance(), 8);

	Pool_Link(Pool_GetInstance(), slave,  master);
	Pool_Link(Pool_GetInstance(), slave2, master);
	Pool_Link(Pool_GetInstance(), slave3, master);

	size_t size = Pool_Free(Pool_GetInstance(), master);

	Assert($("Free"), size == 32 + 3 * 8);
}

tsCase(Acute, "Linking") {
	void *master = Pool_Alloc(Pool_GetInstance(), 32);

	void *slave  = Pool_Alloc(Pool_GetInstance(), 8);
	void *slave2 = Pool_Alloc(Pool_GetInstance(), 8);
	void *slave3 = Pool_Alloc(Pool_GetInstance(), 8);

	/* Note that only one parent can be set. */
	Pool_Link(Pool_GetInstance(), slave,  master);
	Pool_Link(Pool_GetInstance(), slave2, master);
	Pool_Link(Pool_GetInstance(), slave3, master);

	/* Reallocate the first slave in order to verify that it's also updated in
	 * the parent allocation.
	 */
	(void) Pool_Realloc(Pool_GetInstance(), slave, 16);

	size_t size = Pool_Free(Pool_GetInstance(), master);

	Assert($("Free"), size == 32 + 16 + 2 * 8);
}

tsCase(Acute, "Bundling") {
	Pool_Bundle(Pool_GetInstance(), $("Bundle"));

	void *temp = Pool_Alloc(Pool_GetInstance(), 8);
	Pool_Free(Pool_GetInstance(), temp);

	/* As the previous allocation is already destroyed, this will be the
	 * master.
	 */
	void *master = Pool_Alloc(Pool_GetInstance(), 32);

	(void) Pool_Alloc(Pool_GetInstance(), 8);
	(void) Pool_Alloc(Pool_GetInstance(), 8);
	(void) Pool_Alloc(Pool_GetInstance(), 8);

	temp = Pool_Alloc(Pool_GetInstance(), 8);
	Pool_Free(Pool_GetInstance(), temp);

	Pool_Commit(Pool_GetInstance());

	size_t size = Pool_Free(Pool_GetInstance(), master);

	Assert($("Free"), size == 32 + 3 * 8);
}

tsCase(Acute, "Sessions") {
	Pool_Session *sess = Pool_CreateSession(Pool_GetInstance(), $(""), NULL);

	(void) Pool_SetSession(Pool_GetInstance(), sess);

	void *tmp = Pool_Alloc(Pool_GetInstance(), 8);
	Pool_Free(Pool_GetInstance(), tmp);

	(void) Pool_Alloc(Pool_GetInstance(), 8);
	(void) Pool_Alloc(Pool_GetInstance(), 8);
	(void) Pool_Alloc(Pool_GetInstance(), 8);

	size_t size = Pool_Dispose(Pool_GetInstance(), sess);

	Assert($("Free"), size == 3 * 8);
}

tsCase(Acute, "Nested Sessions") {
	Pool_Session *root  = Pool_CreateSession(Pool_GetInstance(), $("Root"),  NULL);
	Pool_Session *child = Pool_CreateSession(Pool_GetInstance(), $("Child"), root);

	(void) Pool_SetSession(Pool_GetInstance(), root);

	(void) Pool_Alloc(Pool_GetInstance(), 2);
	(void) Pool_Alloc(Pool_GetInstance(), 4);

	Pool_Session *result = Pool_SetSession(Pool_GetInstance(), child);
	Assert($("Parent"), root == result);

	(void) Pool_Alloc(Pool_GetInstance(), 6);
	(void) Pool_Alloc(Pool_GetInstance(), 8);

	size_t size = Pool_Dispose(Pool_GetInstance(), root);

	Assert($("Free"), size == 2 + 4 + 6 + 8);
}

tsFinalize;
