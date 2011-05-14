#import "../Stream.h"
#import "../String.h"
#import "../Ecriture.h"
#import "../Exception.h"

#define self Ecriture_Builder

class {
	Stream stream;
	bool flush;
};

rsdef(self, New, Stream stream);
def(void, Destroy);
def(void, ProcessToken, Ecriture_TokenType type, RdString value);

#undef self
