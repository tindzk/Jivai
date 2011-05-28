#import <Types.h>

#import "TestSuite.h"

#define self tsTypes

class {

};

tsRegister("Types") {
	return true;
}

/* See also ccan/short_types/short_types.h. */

tsCase(Acute, "Explicitly-sized types") {
	Assert($("s8"),  sizeof(s8)  == 1);
	Assert($("u8"),  sizeof(u8)  == 1);

	Assert($("s16"), sizeof(s16) == 2);
	Assert($("u16"), sizeof(u16) == 2);

	Assert($("s32"), sizeof(s32) == 4);
	Assert($("u32"), sizeof(u32) == 4);

	Assert($("s64"), sizeof(s64) == 8);
	Assert($("u64"), sizeof(u64) == 8);
}

tsCase(Acute, "Big-endian types") {
	Assert($("be16"), sizeof(be16) == 2);
	Assert($("be32"), sizeof(be32) == 4);
	Assert($("be64"), sizeof(be64) == 8);
}

tsCase(Acute, "Little-endian types") {
	Assert($("le16"), sizeof(le16) == 2);
	Assert($("le32"), sizeof(le32) == 4);
	Assert($("le64"), sizeof(le64) == 8);
}

tsCase(Acute, "Signedness") {
	Assert($("u8"),   (u8)-1 > 0);
	Assert($("u16"), (u16)-1 > 0);
	Assert($("u32"), (u32)-1 > 0);
	Assert($("u64"), (u64)-1 > 0);

	Assert($("s8"),   (s8)-1 < 0);
	Assert($("s16"), (s16)-1 < 0);
	Assert($("s32"), (s32)-1 < 0);
	Assert($("s64"), (s64)-1 < 0);
}
