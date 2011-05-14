#import "../String.h"
#import "../Ecriture.h"
#import "../Exception.h"

#define self Ecriture_Parser

class {
	Ecriture_OnToken onToken;
	RdString buf;
	size_t line;
	size_t ofs;
};

rsdef(self, New, Ecriture_OnToken onToken);
def(void, Destroy);
def(void, Process, RdString s);

#undef self
