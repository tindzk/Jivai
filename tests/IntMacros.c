#import <Macros.h>

#import "TestSuite.h"

#define self tsIntMacros

class {

};

tsRegister("Integer Macros") {
	return true;
}

tsCase(Acute, "isSigned()") {
	Assert($("s32"), isSigned(s32) == true);
	Assert($("u32"), isSigned(u32) == false);

	Assert($("s64"), isSigned(s64) == true);
	Assert($("u64"), isSigned(u64) == false);
}

tsCase(Acute, "MinSigned()") {
	Assert($("s32"), MinSigned(s32) < 0);
	Assert($("s64"), MinSigned(s64) < 0);
}

tsCase(Acute, "MaxSigned()") {
	Assert($("s32"), MaxSigned(s32) > 0);
	Assert($("s64"), MaxSigned(s64) > 0);
}

tsCase(Acute, "MinUnsigned()") {
	Assert($("u32"), MinUnsigned(u32) == 0);
	Assert($("u64"), MinUnsigned(u64) == 0);
}

tsCase(Acute, "MaxUnsigned()") {
	Assert($("u32"), MaxUnsigned(u32) > 0);
	Assert($("u64"), MaxUnsigned(u64) > 0);
}

tsFinalize;
