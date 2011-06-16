#import <Main.h>
#import <String.h>
#import <Process.h>

#define self Application

size_t running = 0;

def(void, onDone, pid_t pid, int status) {
	running--;
	printf("pid=%i done (left=%i)\n", pid, running);

	if (running == 0) {
		EventLoop_Quit(EventLoop_GetInstance());
	}
}

def(void, exec, RdString secs) {
	Process proc = Process_new($("/usr/bin/sleep"));
	Process_addParameter(&proc, secs);

	pid_t pid = Process_spawn(&proc);

	printf("pid=%i start\n", pid);

	Signal_uponChildTermination(Signal_GetInstance(), pid,
		Signal_OnChildTerminate_For(this, ref(onDone)));

	Process_destroy(&proc);

	running++;
}

def(void, exit, Signal_Type type) {
	String_Print($("Early exit.\n"));
	EventLoop_Quit(EventLoop_GetInstance());
}

def(bool, Run) {
	Signal_listen(Signal_GetInstance());

	Signal_uponTermination(Signal_GetInstance(),
		Signal_OnTerminate_For(this, ref(exit)));

	rpt(10) {
		call(exec, $("5"));
		call(exec, $("4"));
		call(exec, $("3"));
		call(exec, $("2"));
		call(exec, $("1"));
	}

	EventLoop_Run(EventLoop_GetInstance());

	return true;
}
