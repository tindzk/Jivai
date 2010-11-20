#import "../Terminal.h"
#import "../Typography.h"
#import "../StringStream.h"

#undef self
#define self Terminal_Controller

class(self) {
	Terminal *term;
};

def(void, Init, Terminal *term);
def(void, Render, String s, ...);
