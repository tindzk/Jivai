#import <String.h>
#import <Exception.h>
#import <Terminal/Controller.h>

#undef self
#define self TestSuite

set(ref(Level)) {
	ref(Level_Acute),
	ref(Level_Trivial)
};

set(ref(MethodType)) {
	ref(MethodType_Run),
	ref(MethodType_Init),
	ref(MethodType_Destroy),
	ref(MethodType_TestCase),
};

record(ref(Method)) {
	ref(MethodType) type;
	void *addr;

	String name;
	size_t level;
};

record(ref(Interface)) {
	String name;
	size_t size;
	TestSuite_Method *first;
	TestSuite_Method *last;
};

Array_Define(ref(Interface) *, TestSuites);

class(self) {
	Terminal term;
	Terminal_Controller controller;

	TestSuites *suites;

	size_t failure;
	size_t success;

	bool acuteFailed;
};

#define tsSection \
	__section(".data.suite")

#define tsRegister(caption)                       \
	sdef(bool, Run);                              \
	TestSuite_Method tsSection ref(MethodRun) = { \
		.type = TestSuite_MethodType_Run,         \
		.addr = ref(Run)                          \
	};                                            \
	TestSuite_Interface Impl(self) = {            \
		.name  = String(caption),                 \
		.size  = sizeof(self),                    \
		.first = &ref(MethodRun),                 \
		.last  = NULL                             \
	};                                            \
	sdef(bool, Run)

#define tsInit                                     \
	def(void, OnInit);                             \
	TestSuite_Method tsSection ref(MethodInit) = { \
		.type = TestSuite_MethodType_Init,         \
		.addr = ref(OnInit)                        \
	};                                             \
	def(void, OnInit)

#define tsDestroy                                     \
	def(void, OnDestroy);                             \
	TestSuite_Method tsSection ref(MethodDestroy) = { \
		.type = TestSuite_MethodType_Destroy,         \
		.addr = ref(OnDestroy)                        \
	};                                                \
	def(void, OnDestroy)

#define tsCaseName \
	tripleConcat(self, _Case, __LINE__)

#define tsMethodName \
	tripleConcat(self, _MethodCase, __LINE__)

#define tsCaseMethod \
	void tsCaseName(__unused Instance(self) $this, __unused TestSuite *ts)

#define tsCase(caseLevel, descr)                \
	tsCaseMethod;                               \
	TestSuite_Method tsSection tsMethodName = { \
		.type  = TestSuite_MethodType_TestCase, \
		.name  = String(descr),                 \
		.level = TestSuite_Level_ ## caseLevel, \
		.addr  = tsCaseName                     \
	};                                          \
	tsCaseMethod

#define tsFinalize                                                \
	TestSuite_Method ref(MethodLast) tsSection = { };             \
	Constructor {                                                 \
		Impl(self).last = &ref(MethodLast);                       \
		TestSuite_AddSuite(TestSuite_GetInstance(), &Impl(self)); \
	}

#define Assert(descr, expr) \
	TestSuite_Assert(ts, descr, expr)

typedef bool (ref(MethodRun))     ();
typedef void (ref(MethodInit))    (GenericInstance);
typedef void (ref(MethodDestroy)) (GenericInstance);
typedef void (ref(MethodTestCase))(GenericInstance, TestSuiteInstance);

SingletonPrototype(self);

def(void, Init);
def(void, Destroy);
def(void, AddSuite, ref(Interface) *suite);
def(void, Assert, String descr, bool succeeded);
