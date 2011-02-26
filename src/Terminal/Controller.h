#import "../Terminal.h"
#import "../Typography.h"
#import "../StringStream.h"

#define self Terminal_Controller

class {
	Terminal *term;
};

def(void, Init, Terminal *term);
def(void, Render, ProtString s, ...);

#undef self
