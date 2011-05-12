#import "../Ecriture.h"
#import "../Terminal.h"
#import "../StringStream.h"

#define self Terminal_Controller

class {
	Terminal *term;
};

rsdef(self, New, Terminal *term);
def(void, Render, RdString s, ...);

#undef self
