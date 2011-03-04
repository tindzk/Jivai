#import <String.h>
#import <Pool.h>

#import "TestSuite.h"

#define self tsPool

class {

};

tsRegister("Pool") {
	return true;
}

tsCase(Acute, "Bundling") {
	Pool_Session *sess = Pool_CreateSession(Pool_GetInstance(), $(""));

	Pool_Push(Pool_GetInstance(), sess, $("Bundling test"));

	Pool_Bundle(Pool_GetInstance(), $("Bundle"));

	void *ptrSlave0 = Pool_Alloc(Pool_GetInstance(), 8);
	Pool_Free(Pool_GetInstance(), ptrSlave0);

	/* As the previous allocation is already destroyed, this will be the
	 * master.
	 */
	void *ptrMaster = Pool_Alloc(Pool_GetInstance(), 32);

	void *ptrSlave  = Pool_Alloc(Pool_GetInstance(), 8);
	void *ptrSlave2 = Pool_Alloc(Pool_GetInstance(), 8);
	void *ptrSlave3 = Pool_Alloc(Pool_GetInstance(), 8);

	void *ptrSlave4 = Pool_Alloc(Pool_GetInstance(), 8);
	Pool_Free(Pool_GetInstance(), ptrSlave4);

	Pool_Commit(Pool_GetInstance());

	Assert($("Size"),
		Pool_GetSize(Pool_GetInstance(), ptrSlave)  == 8 &&
		Pool_GetSize(Pool_GetInstance(), ptrSlave2) == 8 &&
		Pool_GetSize(Pool_GetInstance(), ptrSlave3) == 8);

	Assert($("Free"),
		Pool_Free(Pool_GetInstance(), ptrMaster) == 32 + 3 * 8);

	Pool_Pop(Pool_GetInstance(), sess);

	(void) Pool_Dispose(Pool_GetInstance(), sess);
}

tsFinalize;
