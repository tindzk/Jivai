#import <Folder.h>

#import "TestSuite.h"

#define self tsFolder

class {

};

tsRegister("Folder") {
	return true;
}

tsCase(Acute, "Listing") {
	Folder folder = Folder_new($("../tests/"));

	size_t iter = 0;

	Folder_Entry entry;
	while (Folder_read(&folder, &entry)) {
		iter++;
	}

	Assert($("Iterations"), iter > 1);
	Assert($("Not empty"),  entry.name.len > 0);

	Folder_destroy(&folder);
}
