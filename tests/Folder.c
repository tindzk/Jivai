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
		if (String_Equals(entry.name, $(__FILE__))) {
			found = true;
		}

		iter++;
	}

	Assert($("Found"),      found);     /* Found this file? */
	Assert($("Iterations"), iter > 10); /* Approximation.   */
	Assert($("Not empty"),  entry.name.len > 0);

	Folder_reset(&folder);

	size_t iter2 = 0;
	while (Folder_read(&folder, &entry)) {
		iter2++;
	}

	Assert($("Reset"), iter == iter2);

	Folder_destroy(&folder);
}

tsCase(Acute, "Resetting") {
	Path_createFolder($("a/"),   false);
	Path_createFolder($("a/b/"), false);
	Path_createFolder($("a/c/"), false);
	Path_createFolder($("a/d/"), false);
	Path_createFolder($("a/e/"), false);

	Folder folder = Folder_new($("a/"));

	size_t iter = 0;

	Folder_Entry entry;
	while (Folder_read(&folder, &entry)) {
		iter++;
	}

	Assert($("Count"), iter == 4);

	Path_deleteFolder($("a/e/"));
	Path_deleteFolder($("a/b/"));

	Folder_reset(&folder);
	iter = 0;

	while (Folder_read(&folder, &entry)) {
		iter++;
	}

	Assert($("Count"), iter == 2);

	Folder_destroy(&folder);

	Path_deleteFolder($("a/d/"));
	Path_deleteFolder($("a/c/"));
	Path_deleteFolder($("a/"));
}
