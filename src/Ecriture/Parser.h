#import "../Stream.h"
#import "../Ecriture.h"

#define self Ecriture_Parser

class {
	Ecriture_OnToken onToken;
	Stream stream;
	size_t line;
};

rsdef(self, New, Ecriture_OnToken onToken);
def(void, Destroy);
def(void, Process, Stream stream);

#undef self
