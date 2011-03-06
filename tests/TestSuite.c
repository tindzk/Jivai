#import <Main.h>
#import "TestSuite.h"

#define self TestSuite

Singleton(self);
SingletonDestructor(self);

rsdef(self, New) {
	self res;

	res.term = Terminal_New(File_StdIn, File_StdOut, true);
	Terminal_Configure(&res.term, true, true);

	res.suites = TestSuites_New(128);
	res.acuteFailed = false;

	return res;
}

def(void, Destroy) {
	Terminal_Destroy(&this->term);
	TestSuites_Free(this->suites);
}

def(void, AddSuite, ITestSuiteInterface *suite) {
	TestSuites_Push(&this->suites, suite);
}

def(void, Assert, ProtString descr, bool succeeded) {
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
	forward(i, suite->last - suite->first) {
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
	forward(i, suite->last - suite->first) {
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

	Terminal_Print(&this->term, '\n');
}

def(bool, Run) {
	this->controller = Terminal_Controller_New(&this->term);

	this->acuteFailed = false;

	foreach (suite, this->suites) {
		bool run = call(RunSuite, *suite);

		Terminal_Controller_Render(&this->controller,
			$(".fg[blue]{.b{% suite .i{%...}}}\n\n"),

			run
				? $("Running")
				: $("Skipping"),

			(*suite)->name);

		if (run) {
			GenericInstance inst =
				((*suite)->size > 0)
					? Generic_New((*suite)->size)
					: Generic_Null();

			this->failure = 0;
			this->success = 0;

			call(InitSuite,    *suite, inst);
			call(RunTestSuite, *suite, inst);
			call(DestroySuite, *suite, inst);

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

		Terminal_Print(&this->term, '\n');
	}

	return !this->acuteFailed;
}

bool Main(__unused ProtString base, __unused ProtStringArray *args) {
	return TestSuite_Run(TestSuite_GetInstance());
}
