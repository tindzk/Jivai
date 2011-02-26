#import "../String.h"

#define self HTTP_Method

set(self) {
	ref(Head),
	ref(Get),
	ref(Post),
	ref(Unset)
};

sdef(self, FromString, ProtString s);
sdef(ProtString, ToString, self method);

#undef self
