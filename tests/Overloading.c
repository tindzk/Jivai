#import <String.h>
#import <Number.h>
#import <Integer.h>

#import "TestSuite.h"
#import "App.h"

#undef self
#define self tsOverloading

class(self) {
	enum {
		MethodInt8,
		MethodInt16,
		MethodInt32,
		MethodInt64,

		MethodUInt8,
		MethodUInt16,
		MethodUInt32,
		MethodUInt64
	} type;
};

tsRegister("Overloading") {
	return true;
}

overload def(void, Foo, __unused s8 val) {
	this->type = MethodInt8;
}

overload def(void, Foo, __unused s16 val) {
	this->type = MethodInt16;
}

overload def(void, Foo, __unused s32 val) {
	this->type = MethodInt32;
}

overload def(void, Foo, __unused s64 val) {
	this->type = MethodInt64;
}

overload def(void, Foo, __unused u8 val) {
	this->type = MethodUInt8;
}

overload def(void, Foo, __unused u16 val) {
	this->type = MethodUInt16;
}

overload def(void, Foo, __unused u32 val) {
	this->type = MethodUInt32;
}

overload def(void, Foo, __unused u64 val) {
	this->type = MethodUInt64;
}

tsCase(Acute, "Overload") {
	call(Foo, -5);
	Assert($("Select `default' function (s32)"),
		this->type == MethodInt32);

	call(Foo, 5);
	Assert($("Select `default' function (s32)"),
		this->type == MethodInt32);

	call(Foo, (s8) 5);
	Assert($("Select s8 function by casting"),
		this->type == MethodInt8);

	/* This value can be represented by using either s64 or u64.
	 * However, Clang chooses s64.
	 */
	call(Foo, -12345678987);
	Assert($("Select s64 function by using a large value"),
		this->type == MethodInt64);

	/* This value can be only represented with s64. */
	call(Foo, 18446744073709551615);
	Assert($("Select u64 function by using a large value"),
		this->type == MethodUInt64);

	u16 val = 1234;
	call(Foo, val);
	Assert($("Select u16 function by passing a variable"),
		this->type == MethodUInt16);

	u32 val2 = 1234;
	call(Foo, val2);
	Assert($("Select u32 function by passing a variable"),
		this->type == MethodUInt32);
}

tsFinalize;
