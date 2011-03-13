#import "../String.h"

#define self HTTP_Method

set(self) {
	ref(Head),
	ref(Get),
	ref(Post),
	ref(Unset)
};

sdef(self, FromString, RdString s);
sdef(RdString, ToString, self method);

#undef self
