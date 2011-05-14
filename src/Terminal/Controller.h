#import "../Terminal.h"
#import "../Ecriture/Parser.h"

#define self Terminal_Controller

#ifndef Terminal_Controller_Depth
#define Terminal_Controller_Depth 5
#endif

record(ref(Item)) {
	RdString name;
	Terminal_Style style;
};

class {
	Terminal *term;
	VarArg argptr;

	ref(Item) items[ref(Depth)];
	size_t depth;
};

rsdef(self, New, Terminal *term);
def(void, Render, RdString s, ...);

#undef self
