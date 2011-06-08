#import <ELF.h>
#import <Main.h>
#import <String.h>
#import <Exception.h>
#import <Application.h>
#import <Terminal/Controller.h>

#define self TestSuite

set(ref(Level)) {
	ref(Level_Acute),
	ref(Level_Trivial)
};

set(ref(Type)) {
	ref(Type_Section) = 1,
	ref(Type_Run),
	ref(Type_Init),
	ref(Type_Destroy),
	ref(Type_TestCase),
};

record(ref(Method)) {
	ref(Type) type;
	void *addr;

	RdString name;
	size_t level;
};

Interface(ITestSuite) {
	ref(Type) type;
	RdString name;
	size_t size;
};

class {
	Terminal *term;
	Terminal_Controller controller;

	bool found;

	RdString name;

	size_t failure;
	size_t success;

	bool acuteFailed;
};

#define __tsSection(x) \
	".suite." #x

#define _tsSection(x) \
	__tsSection(x)

#define tsSection \
	__section(_tsSection(self))

#define tsRegister(caption)                       \
	tsSection Impl(ITestSuite) = {                \
		.type = TestSuite_Type_Section,           \
		.name = $(caption),                       \
		.size = sizeof(self)                      \
	};                                            \
	sdef(bool, Run);                              \
	TestSuite_Method tsSection ref(MethodRun) = { \
		.type = TestSuite_Type_Run,               \
		.addr = ref(Run)                          \
	};                                            \
	sdef(bool, Run)

#define tsInit                                     \
	def(void, OnInit);                             \
	TestSuite_Method tsSection ref(MethodInit) = { \
		.type = TestSuite_Type_Init,               \
		.addr = ref(OnInit)                        \
	};                                             \
	def(void, OnInit)

#define tsDestroy                                     \
	def(void, OnDestroy);                             \
	TestSuite_Method tsSection ref(MethodDestroy) = { \
		.type = TestSuite_Type_Destroy,               \
		.addr = ref(OnDestroy)                        \
	};                                                \
	def(void, OnDestroy)

#define tsCaseName \
	tripleConcat(self, _Case, __LINE__)

#define tsMethodName \
	tripleConcat(self, _MethodCase, __LINE__)

#define tsCaseMethod \
	void tsCaseName(__unused DynInstName(self) $this, __unused TestSuite *ts)

#define tsCase(caseLevel, descr)                \
	tsCaseMethod;                               \
	TestSuite_Method tsSection tsMethodName = { \
		.type  = TestSuite_Type_TestCase,       \
		.name  = $(descr),                      \
		.level = TestSuite_Level_ ## caseLevel, \
		.addr  = tsCaseName                     \
	};                                          \
	tsCaseMethod

#define Assert(descr, expr) \
	TestSuite_Assert(ts, descr, expr)

typedef bool (ref(MethodRun))     ();
typedef void (ref(MethodInit))    (Instance $this);
typedef void (ref(MethodDestroy)) (Instance $this);
typedef void (ref(MethodTestCase))(Instance $this, TestSuiteInst ts);

SingletonPrototype(self);

rsdef(self, New);
def(void, Destroy);
def(void, Assert, RdString descr, bool succeeded);

#undef self
