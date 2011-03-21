#import <Directory.h>

#import "TestSuite.h"

#define self tsDirectory

class {

};

tsRegister("Directory") {
	return true;
}

tsCase(Acute, "Listing") {
	Directory dir;
	Directory_Init(&dir, $("../tests"));

	size_t iter = 0;

	Directory_Entry entry;
	while (Directory_Read(&dir, &entry)) {
		iter++;
	}

	Assert($("Loops"),     iter > 1);
	Assert($("Not empty"), entry.name.len > 0);

	Directory_Destroy(&dir);
}

tsFinalize;
