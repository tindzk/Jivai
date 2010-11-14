#import "String.h"
#import "Charset.h"

#undef self
#define self Unicode

sdef(size_t, CalcWidth, const char *src);
sdef(size_t, Next, String s, size_t offset);
sdef(size_t, Prev, String s, size_t offset);
overload sdef(size_t, Count, String s, size_t offset, size_t len);
overload sdef(size_t, Count, String s);
sdef(void, Shrink, String *s, size_t len);
sdef(void, ToMultiByte, int c, String *res);
