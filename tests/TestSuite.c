#import <Main.h>
#import "TestSuite.h"

#define self TestSuite

Singleton(self);
SingletonDestructor(self);

rsdef(self, New) {
	return (self) {
		.acuteFailed = false
	};
}

def(void, Destroy) { }

def(void, SetTerminal, Terminal *term) {
	this->term       = term;
	this->controller = Terminal_Controller_New(term);
}

def(void, Assert, RdString descr, bool succeeded) {
	if (succeeded) {
		this->success++;
	} else {
		this->failure++;
	}

	Terminal_Controller_Render(&this->controller,
		$("   % .fg[%]{%}\n"),

		succeeded
			? $("✓")
			: $("✗"),

		succeeded
			? $("green")
			: $("red"),

		descr);
}

static def(void *, Resolve, RdBuffer methods, ref(Type) type) {
	for (TestSuite_Method *cur = methods.ptr; (void *) cur < methods.ptr + methods.len; cur++) {
		if (cur->type == type) {
			return cur->addr;
		}
	}

	return NULL;
}

static inline def(bool, RunSuite, RdBuffer methods) {
	ref(MethodRun) *method = call(Resolve, methods, ref(Type_Run));

	return (method != NULL)
		? method()
		: false;
}

static inline def(void, InitSuite, RdBuffer methods, DynObject inst) {
	ref(MethodInit) *method = call(Resolve, methods, ref(Type_Init));

	if (method != NULL) {
		method(inst.addr);
	}
}

static inline def(void, DestroySuite, RdBuffer methods, DynObject inst) {
	ref(MethodDestroy) *method = call(Resolve, methods, ref(Type_Destroy));

	if (method != NULL) {
		method(inst.addr);
	}
}

static inline def(void, RunTestSuite, RdBuffer methods, DynObject inst) {
	for (TestSuite_Method *cur = methods.ptr; (void *) cur < methods.ptr + methods.len; cur++) {
		if (cur->type == ref(Type_TestCase)) {
			Terminal_Controller_Render(&this->controller,
				$(" % .fg[yellow]{.b{Test case:} .i{%}}\n"),

				cur->level == ref(Level_Trivial)
					? $("·")
					: $("★"),

				cur->name);

			((TestSuite_MethodTestCase *) cur->addr)(inst.addr, this);

			if (cur->level == ref(Level_Acute)) {
				if (this->failure > 0) {
					Terminal_Controller_Render(&this->controller,
						$("\n   .fg[red]{.b{Omitting all remaining test cases.}}\n"));

					this->acuteFailed = true;

					break;
				}
			}
		}
	}

	Terminal_Print(this->term, '\n');
}

def(bool, OnSection, __unused RdString name, RdBuffer sect) {
	assert(sect.len > sizeof(ITestSuiteInterface));

	ITestSuiteInterface *suite = sect.ptr;

	assert(suite->type == ref(Type_Section));

	if (this->name.len > 0) {
		if (String_Equals(suite->name, this->name)) {
			this->found = true;
		} else {
			return true;
		}
	}

	RdBuffer methods = {
		.ptr = sect.ptr + sizeof(ITestSuiteInterface),
		.len = sect.len - sizeof(ITestSuiteInterface)
	};

	bool run = call(RunSuite, methods);

	Terminal_Controller_Render(&this->controller,
		$(".fg[blue]{.b{% suite .i{%...}}}\n\n"),

		run
			? $("Running")
			: $("Skipping"),

		suite->name);

	if (run) {
		this->failure = 0;
		this->success = 0;

		DynObject object = DynObject_New(suite->size);

		call(InitSuite,    methods, object);
		call(RunTestSuite, methods, object);
		call(DestroySuite, methods, object);

		String strSuccess = Integer_ToString(this->success);
		String strFailure = Integer_ToString(this->failure);

		Terminal_Controller_Render(&this->controller,
			$(".fg[cyan]{.u{Results:} .fg[green]{%} succeeeded, .fg[red]{%} failed}\n"),
			strSuccess, strFailure);

		String_Destroy(&strFailure);
		String_Destroy(&strSuccess);

		DynObject_Destroy(object);
	}

	Terminal_Print(this->term, '\n');

	return this->name.len == 0;
}

overload def(void, Run) {
	ELF elf = ELF_New($("./TestSuite.bin")); /* TODO */
	ELF_Each(&elf, $(".suite."), ELF_OnSection_For(this, ref(OnSection)));
	ELF_Destroy(&elf);
}

overload def(void, Run, RdString name) {
	this->name = name;
	call(Run);

	if (!this->found) {
		Terminal_Controller_Render(&this->controller,
			$(".fg[red]{.b{Error:} Test suite .i{%} not found!}\n"),
			name);
	}
}

def(bool, Successful) {
	return !this->acuteFailed;
}

#undef self

#define self Application

def(bool, Run) {
	Terminal_Configure(&this->term, true, true);

	TestSuite *inst = TestSuite_GetInstance();

	TestSuite_SetTerminal(inst, &this->term);

	if (this->args->len == 0) {
		TestSuite_Run(inst);
	} else {
		fwd(i, this->args->len) {
			TestSuite_Run(inst, this->args->buf[i]);
		}
	}

	return TestSuite_Successful(inst);
}
