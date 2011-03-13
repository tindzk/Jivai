#import "../String.h"

#define self HTTP_Version

set(self) {
	ref(1_0),
	ref(1_1),
	ref(Unset)
};

sdef(RdString, ToString, self version);
sdef(self, FromString, RdString s);

#undef self
