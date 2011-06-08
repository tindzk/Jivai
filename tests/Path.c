#import <Path.h>

#import "TestSuite.h"

#define self tsPath

class {

};

tsRegister("Path") {
	return true;
}

tsCase(Acute, "Extracting extension") {
	RdString ext = Path_GetExtension($("/usr/bin/test"));
	Assert($("Equals"), String_Equals(ext, $("")));

	ext = Path_GetExtension($("/usr/bin/test.exe"));
	Assert($("Equals"), String_Equals(ext, $("exe")));

	ext = Path_GetExtension($("/usr/bin.2/test"));
	Assert($("Equals"), String_Equals(ext, $("")));
}
