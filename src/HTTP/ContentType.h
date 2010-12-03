#import "../Macros.h"

#define self HTTP_ContentType

set(self) {
	ref(Unset),
	ref(SinglePart),
	ref(MultiPart)
};

#undef self
