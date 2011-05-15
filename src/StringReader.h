#import "String.h"
#import "Exception.h"

#define self StringReader

class {
	size_t ofs;
	size_t line;
	RdString buf;
};

rsdef(self, New, RdString s);
overload def(bool, Peek, char *c);
overload def(bool, Peek, char *c, size_t cnt);
overload def(void, Consume);
def(void, Extend, RdString *str);

static alwaysInline rdef(size_t, GetLine) {
	return this->line;
}

#undef self
