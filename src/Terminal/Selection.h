#import "Buffer.h"

#undef self
#define self Terminal_Selection

class(self) {
	Terminal *term;
	Terminal_Buffer termbuf;
	size_t cur;
};

def(void, Init, Terminal *term);
def(void, Destroy);
def(void, OnSelect, size_t id);
def(void, Add, String caption, bool selected);
def(ssize_t, GetSel);
