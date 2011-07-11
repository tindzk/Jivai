#import <File.h>

#import "TestSuite.h"

#define self tsFile

class {

};

tsRegister("File") {
	return true;
}

tsCase(Acute, "Meta information") {
	File file = File_new($("./TestSuite.exe"), FileStatus_ReadOnly);

	size_t size = 1024 * 250;
	Stat64 meta = File_getMeta(&file);
	Assert($("Size approximation"), meta.size > size);

	File_destroy(&file);
}

