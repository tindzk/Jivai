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
	bool found = false;

	Folder_Entry entry;
	while (Folder_read(&folder, &entry)) {
		if (String_Equals(entry.name, $("Folder.c"))) {
			found = true;
		}

		iter++;
	}

	Assert($("Found"),      found);     /* Found this file? */
	Assert($("Iterations"), iter > 10); /* Approximation.   */
	Assert($("Not empty"),  entry.name.len > 0);

	Folder_destroy(&folder);
}
