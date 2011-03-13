#import <Main.h>
#import "TestSuite.h"

#define self TestSuite

Singleton(self);
SingletonDestructor(self);

rsdef(self, New) {
	return (self) {
		.suites = TestSuites_New(128),
		.acuteFailed = false
	};
}

def(void, Destroy) {
	TestSuites_Free(this->suites);
}

def(void, SetTerminal, Terminal *term) {
	this->term       = term;
	this->controller = Terminal_Controller_New(term);
}

def(void, AddSuite, ITestSuiteInterface *suite) {
	TestSuites_Push(&this->suites, suite);
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

static def(void *, Resolve, ITestSuiteInterface *suite, ref(MethodType) type) {
	fwd(i, suite->last - suite->first) {
		ref(Method) *method = &suite->first[i];

		if (method->type == type) {
			return method->addr;
		}
	}

	return NULL;
}

static def(bool, RunSuite, ITestSuiteInterface *suite) {
	ref(MethodRun) *method = call(Resolve, suite, ref(MethodType_Run));

	return (method != NULL)
		? method()
		: false;
}

static def(void, InitSuite, ITestSuiteInterface *suite, GenericInstance inst) {
	ref(MethodInit) *method = call(Resolve, suite, ref(MethodType_Init));

	if (method != NULL) {
		method(inst);
	}
}

static def(void, DestroySuite, ITestSuiteInterface *suite, GenericInstance inst) {
	ref(MethodDestroy) *method = call(Resolve, suite, ref(MethodType_Destroy));

	if (method != NULL) {
		method(inst);
	}
}

static def(void, RunTestSuite, ITestSuiteInterface *suite, GenericInstance inst) {
	fwd(i, suite->last - suite->first) {
		ref(Method) *method = &suite->first[i];

		if (method->type == ref(MethodType_TestCase)) {
			Terminal_Controller_Render(&this->controller,
				$(" % .fg[yellow]{.b{Test case:} .i{%}}\n"),

				method->level == ref(Level_Trivial)
					? $("·")
					: $("★"),

				method->name);

			((TestSuite_MethodTestCase *) method->addr)(inst, this);

			if (method->level == ref(Level_Acute)) {
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

def(ITestSuiteInterface *, ResolveSuite, RdString name) {
	each(suite, this->suites) {
		if (String_Equals((*suite)->name, name)) {
			return *suite;
		}
	}

	return NULL;
}

def(void, _Run, ITestSuiteInterface *suite) {
	bool run = call(RunSuite, suite);

	Terminal_Controller_Render(&this->controller,
		$(".fg[blue]{.b{% suite .i{%...}}}\n\n"),

		run
			? $("Running")
			: $("Skipping"),

		suite->name);

	if (run) {
		GenericInstance inst =
			(suite->size > 0)
				? Generic_New(suite->size)
				: Generic_Null();

		this->failure = 0;
		this->success = 0;

		call(InitSuite,    suite, inst);
		call(RunTestSuite, suite, inst);
		call(DestroySuite, suite, inst);

		String strSuccess = Integer_ToString(this->success);
		String strFailure = Integer_ToString(this->failure);

		Terminal_Controller_Render(&this->controller,
			$(".fg[cyan]{.u{Results:} .fg[green]{%} succeeeded, .fg[red]{%} failed}\n"),
			strSuccess, strFailure);

		String_Destroy(&strFailure);
		String_Destroy(&strSuccess);

		if (!Generic_IsNull(inst)) {
			Generic_Free(inst);
		}
	}

	Terminal_Print(this->term, '\n');
}

def(void, Run, RdString name) {
	ITestSuiteInterface *suite = call(ResolveSuite, name);

	if (suite == NULL) {
		Terminal_Controller_Render(&this->controller,
			$(".fg[red]{.b{Error:} Test suite .i{%} not found!}\n"),
			name);

		return;
	}

	call(_Run, suite);
}

def(void, RunAll) {
	each(suite, this->suites) {
		call(_Run, *suite);
	}
}

def(bool, Successful) {
	return !this->acuteFailed;
}

#undef self

#define self Application

def(bool, Run) {
	Terminal_Configure(&this->term, true, true);

	TestSuiteInstance inst = TestSuite_GetInstance();

	TestSuite_SetTerminal(inst, &this->term);

	if (this->args->len == 0) {
		TestSuite_RunAll(inst);
	} else {
		fwd(i, this->args->len) {
			TestSuite_Run(inst, this->args->buf[i]);
		}
	}

	return TestSuite_Successful(inst);
}
