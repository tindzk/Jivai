#import "Module.h"

static size_t cnt = 0;

static Module modules[Module_Approximation];

size_t Module_Register(String name) {
	modules[cnt] = (Module) {
		.name = name
	};

	return cnt++;
}

String Module_ResolveName(size_t module) {
	if (module > cnt) {
		return String("");
	}

	return modules[module].name;
}
