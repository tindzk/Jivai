#import "../String.h"
#import "../Ecriture.h"
#import "../StringReader.h"

#define self Ecriture_Parser

class {
	Ecriture_OnToken onToken;
	StringReader reader;
};

rsdef(self, New, Ecriture_OnToken onToken);
def(void, Destroy);
def(void, Process, RdString s);

#undef self
