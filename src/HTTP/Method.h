#import "../String.h"

#undef self
#define self HTTP_Method

set(self) {
	ref(Head),
	ref(Get),
	ref(Post),
	ref(Unset)
};

sdef(self, FromString, String s);
sdef(String, ToString, self method);
