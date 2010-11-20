#import "TestSuite.h"
#import "App.h"

ExceptionManager exc;

Singleton(self);
SingletonDestructor(self);

def(void, Init) {
	Terminal_Init(&this->term, File_StdIn, File_StdOut, true);
	Terminal_Configure(&this->term, true, true);

	Terminal_Controller_Init(&this->controller, &this->term);

	this->suites = TestSuites_New(128);

	this->acuteFailed = false;
}

def(void, Destroy) {
	Terminal_Destroy(&this->term);

	TestSuites_Free(this->suites);
}

def(void, AddSuite, ref(Interface) *suite) {
	TestSuites_Push(&this->suites, suite);
}

def(void, Assert, String descr, bool succeeded) {
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

def(void *, Resolve, ref(Interface) *suite, ref(MethodType) type) {
	forward(i, suite->last - suite->first) {
		ref(Method) *method = &suite->first[i];

		if (method->type == type) {
			return method->addr;
		}
	}

	return NULL;
}

def(bool, RunSuite, ref(Interface) *suite) {
	ref(MethodRun) *method = call(Resolve, suite, ref(MethodType_Run));

	return (method != NULL)
		? method()
		: false;
}

def(void, InitSuite, ref(Interface) *suite, GenericInstance inst) {
	ref(MethodInit) *method = call(Resolve, suite, ref(MethodType_Init));

	if (method != NULL) {
		method(inst);
	}
}

def(void, DestroySuite, ref(Interface) *suite, GenericInstance inst) {
	ref(MethodDestroy) *method = call(Resolve, suite, ref(MethodType_Destroy));

	if (method != NULL) {
		method(inst);
	}
}

def(void, RunTestSuite, ref(Interface) *suite, GenericInstance inst) {
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

			Terminal_Controller_Render(&this->controller,
				$(".fg[cyan]{.u{Results:} .fg[green]{%} succeeeded, .fg[red]{%} failed}\n"),
				Integer_ToString(this->success),
				Integer_ToString(this->failure));

			if (!Generic_IsNull(inst)) {
				Generic_Free(inst);
			}
		}

		Terminal_Print(&this->term, '\n');
	}

	return !this->acuteFailed;
}

int main(void) {
	ExceptionManager_Init(&exc);

	String0(&exc);
	Memory0(&exc);
	Integer0(&exc);

	return TestSuite_Run(TestSuite_GetInstance())
		? ExitStatus_Success
		: ExitStatus_Failure;
}