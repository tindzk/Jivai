#import "../String.h"

#define self HTTP_Version

set(self) {
	ref(1_0),
	ref(1_1),
	ref(Unset)
};

sdef(self, FromString, String s);

#undef self
