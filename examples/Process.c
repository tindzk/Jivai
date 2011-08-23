#import <Main.h>
#import <String.h>
#import <Process.h>

#define self Application

size_t running = 0;

def(void, onDone, pid_t pid, u8 status) {
	Process_suspend(pid);
	running--;

	if (status == Process_SpawningProcessFailed) {
		printf("pid=%i error (left=%i)\n", pid, running);
	} else {
		printf("pid=%i done status=%i (left=%i)\n", pid, status, running);
	}

	if (running == 0) {
		EventLoop_quit(EventLoop_GetInstance());
	}
}

def(void, exec, bool err, RdString secs) {
	Process proc = Process_new(err ? $("/err") : $("/usr/bin/sleep"));
	Process_addParameter(&proc, secs);

	pid_t pid = Process_spawn(&proc);

	printf("pid=%i start\n", pid);

	Signal_uponChildTermination(Signal_GetInstance(), pid,
		Signal_OnChildTerminate_For(this, ref(onDone)));

	Process_destroy(&proc);

	running++;
}

def(void, exit, Signal_Type type) {
	System_out($("Early exit.\n"));
	EventLoop_quit(EventLoop_GetInstance());
}

def(bool, Run) {
	Signal_listen(Signal_GetInstance());

	Signal_uponTermination(Signal_GetInstance(),
		Signal_OnTerminate_For(this, ref(exit)));


	rpt(1) {
		call(exec, true, $(""));
		call(exec, false, $("5"));
		call(exec, false, $("4"));
		call(exec, false, $("3"));
		call(exec, false, $("2"));
		call(exec, false, $("1"));
	}

	EventLoop_run(EventLoop_GetInstance());

	return true;
}
