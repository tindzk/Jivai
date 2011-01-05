#import <Memory.h>

#import "TestSuite.h"

#define self tsMemory

class {

};

tsRegister("Memory") {
	return true;
}

char test = 'a';
char *abc = "hello world";

tsCase(Acute, "Detect read-only data") {
	void *a = alloca(1);
	char b;
	void *ptr = Memory_Alloc(32);

	Assert($("Global constant data"),
		Memory_IsRoData(&test) == false);

	Assert($("Global char pointer"),
		Memory_IsRoData(abc) == true);

	Assert($("Local uninitialised variable"),
		Memory_IsRoData(&b) == false);

	Assert($("Local char pointer"),
		Memory_IsRoData("abc") == true);

	Assert($("Stack memory"),
		Memory_IsRoData(a) == false);

	Assert($("Stack memory"),
		Memory_IsRoData(alloca(1)) == false);

	Assert($("Heap memory"),
		Memory_IsRoData(ptr) == false);

	Memory_Free(ptr);
}

tsFinalize;
