#import <Memory.h>

#import "TestSuite.h"

#define self tsMemory

class {

};

tsRegister("Memory") {
	return true;
}

char data1   = 'a';
char data2[] = "hello world";
char *rodata = "hello world";

tsCase(Acute, "Detect read-only data") {
	char c;
	void *stack = alloca(1);
	void *heap = Memory_Alloc(32);

	Assert($("Global constant data"),
		Memory_IsRoData(&data1) == false);

	Assert($("Global constant data"),
		Memory_IsRoData(data2) == false);

	Assert($("Global rodata char pointer"),
		Memory_IsRoData(rodata) == true);

	Assert($("Local uninitialised variable"),
		Memory_IsRoData(&c) == false);

	Assert($("Local char pointer"),
		Memory_IsRoData("abc") == true);

	Assert($("Stack memory"),
		Memory_IsRoData(stack) == false);

	Assert($("Stack memory"),
		Memory_IsRoData(alloca(1)) == false);

	Assert($("Heap memory"),
		Memory_IsRoData(heap) == false);

	Memory_Free(heap);
}

tsFinalize;
