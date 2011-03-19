#import "Buffer.h"

#define self Terminal_Selection

class {
	Terminal *term;
	Terminal_Buffer termbuf;
	size_t cur;
};

rsdef(self, New, Terminal *term);
def(void, Destroy);
def(void, OnSelect, size_t id);
def(void, Add, RdString caption, bool selected);
def(ssize_t, GetSel);

#undef self
