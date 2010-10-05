#define __paste(x, y) \
	x ## _ ## y

#define __eval(x, y) \
	__paste(x, y)

#define ref(name) \
	__eval(self, name)

#define def(ret, name, ...) \
	ret ref(name)(self *restrict this, ## __VA_ARGS__)
