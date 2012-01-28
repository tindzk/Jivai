#import <Main.h>
#import <String.h>
#import <Process.h>

#define self Application

record(ref(Process)) {
	self *inst;
	Process_Data *data;
	EventLoop_Entry *entry;
};

static size_t running = 0;

sdef(void, onDone, Instance inst, pid_t pid, u8 status) {
	ref(Process) *procItem = inst.addr;

	running--;

	printf("status=%i\n", status);

	if (status == Process_SpawningProcessFailed) {
		printf("pid=%i error (left=%i)\n", pid, running);
	} else {
		printf("pid=%i done status=%i (left=%i)\n", pid, status, running);
	}

	EventLoop_enqueue(EventLoop_GetInstance(),
		procItem->entry, ChannelWatcher_Events_HangUp);

	if (running == 0) {
		EventLoop_quit(EventLoop_GetInstance());
	}
}

static sdef(void, onDestroy, Instance inst) {
	ref(Process) *procItem = inst.addr;

	printf("onDestroy(%i)\n", procItem->entry->ch->id);
	fflush(stdout);

	Process_Data *data = procItem->data;

	/* The last parameter must be `true'. */
	EventLoop_detach(EventLoop_GetInstance(), procItem->entry, true);

	Process_Data_destroy(data);
}

static sdef(void, onData, Instance inst) {
	ref(Process) *procItem = inst.addr;

	printf("onData fd=%i\n", procItem->entry->ch->id);
	fflush(stdout);

	String s = String_New(1024);

	for (;;) {
		s.len = Channel_Read(procItem->entry->ch, s.buf, String_GetSize(s));

		if (s.len == 0) {
			break;
		}

		System_out(s.rd);
	}

	String_Destroy(&s);
}

def(void, listen, Process *proc, Process_Data *data) {
	EventLoop_Entry *entry =
		EventLoop_createEntry(EventLoop_GetInstance(), this,
			sizeof(ref(Process)));

	ref(Process) *procItem = (void *) entry->data;

	procItem->data  = data;
	procItem->entry = entry;

	EventLoop_Options opts = {
		.ch            = &data->ch,
		.edgeTriggered = false,
		.events        = {
			.inst      = { .addr = procItem },
			.onDestroy = ref(onDestroy),
			.onInput   = ref(onData)
		}
	};

	EventLoop_attach(EventLoop_GetInstance(), entry, opts);

	Signal_uponChildTermination(Signal_GetInstance(), data->pid,
		Signal_OnChildTerminate_For(procItem, ref(onDone)));
}

def(void, exec, RdString param) {
	Process proc = Process_new($("/bin/echo"));
	Process_setPipe(&proc, Process_Pipe_Read);
	Process_addParameter(&proc, param);

	Process_Data *data = Process_spawn(&proc);

	/* Listen for input on pipe. */
	call(listen, &proc, data);

	printf("pid=%i start\n", data->pid);

	Process_destroy(&proc);

	running++;
}

def(void, exit, Signal_Type type) {
	System_out($("Early exit.\n"));
	/* @todo Destroy all open processes. */
	EventLoop_quit(EventLoop_GetInstance());
}

def(bool, Run) {
	Signal_listen(Signal_GetInstance());

	Signal_uponTermination(Signal_GetInstance(),
		Signal_OnTerminate_For(this, ref(exit)));

	call(exec, $("a"));
	call(exec, $("b"));
	call(exec, $("c"));
	call(exec, $("d"));

	EventLoop_run(EventLoop_GetInstance());

	return true;
}
