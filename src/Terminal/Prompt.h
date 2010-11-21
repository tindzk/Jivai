#import "Buffer.h"

#undef self
#define self Terminal_Prompt

class(self) {
	Terminal *term;
	Terminal_Buffer termbuf;
};

def(void, Init, Terminal *term);
def(void, Destroy);
def(bool, Ask, String msg);
