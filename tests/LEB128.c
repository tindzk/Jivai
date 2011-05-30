#import <LEB128.h>

#import "TestSuite.h"

/* The values were taken from the DWARF 4 specification. Refer to
 * section "7.6 - Variable Length Data".
 */

#define self tsLEB128

class {

};

tsRegister("LEB128") {
	return true;
}

tsCase(Acute, "Unsigned") {
	u32 ret;

	Assert($("Result"), LEB128_ReadUnsigned((ubyte[]) { 2 }, &ret) == 1);
	Assert($("Value"),  ret == 2);

	Assert($("Result"), LEB128_ReadUnsigned((ubyte[]) { 127 }, &ret) == 1);
	Assert($("Value"),  ret == 127);

	Assert($("Result"), LEB128_ReadUnsigned((ubyte[]) { 0 + 0x80, 1 }, &ret) == 2);
	Assert($("Value"),  ret == 128);

	Assert($("Result"), LEB128_ReadUnsigned((ubyte[]) { 1 + 0x80, 1 }, &ret) == 2);
	Assert($("Value"),  ret == 129);

	Assert($("Result"), LEB128_ReadUnsigned((ubyte[]) { 2 + 0x80, 1 }, &ret) == 2);
	Assert($("Value"),  ret == 130);

	Assert($("Result"), LEB128_ReadUnsigned((ubyte[]) { 57 + 0x80, 100 }, &ret) == 2);
	Assert($("Value"),  ret == 12857);
}

tsCase(Acute, "Signed") {
	s32 ret;

	Assert($("Result"), LEB128_ReadSigned((ubyte[]) { 2 }, &ret) == 1);
	Assert($("Value"),  ret == 2);

	Assert($("Result"), LEB128_ReadSigned((ubyte[]) { 0x7e }, &ret) == 1);
	Assert($("Value"),  ret == -2);

	Assert($("Result"), LEB128_ReadSigned((ubyte[]) { 127 + 0x80, 0 }, &ret) == 2);
	Assert($("Value"),  ret == 127);

	Assert($("Result"), LEB128_ReadSigned((ubyte[]) { 1 + 0x80, 0x7f }, &ret) == 2);
	Assert($("Value"),  ret == -127);

	Assert($("Result"), LEB128_ReadSigned((ubyte[]) { 0 + 0x80, 1 }, &ret) == 2);
	Assert($("Value"),  ret == 128);

	Assert($("Result"), LEB128_ReadSigned((ubyte[]) { 0 + 0x80, 0x7f }, &ret) == 2);
	Assert($("Value"),  ret == -128);

	Assert($("Result"), LEB128_ReadSigned((ubyte[]) { 1 + 0x80, 1 }, &ret) == 2);
	Assert($("Value"),  ret == 129);

	Assert($("Result"), LEB128_ReadSigned((ubyte[]) { 0x7f + 0x80, 0x7e }, &ret) == 2);
	Assert($("Value"),  ret == -129);
}

