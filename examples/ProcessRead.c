/* Spawns one process and receives its output. */

#import <Main.h>
#import <String.h>
#import <Process.h>

#define self Application

record(ref(Process)) {
	self *inst;
	Process_Data *data;
	EventLoop_Entry *entry;
};

ref(Process) *process = null;

sdef(void, onDone, Instance inst, pid_t pid, u8 status) {
	process = null;

	ref(Process) *procItem = inst.addr;

	if (status == Process_SpawningProcessFailed) {
		printf("pid=%i error\n", pid);
	} else {
		printf("pid=%i done status=%i\n", pid, status);
	}

	fflush(stdout);

	/* Unregister the process safely. This will implicitly call
	 * onDestroy().
	 */
	EventLoop_enqueue(EventLoop_GetInstance(),
		procItem->entry, ChannelWatcher_Events_HangUp);

	EventLoop_quit(EventLoop_GetInstance());
}

static sdef(void, onDestroy, Instance inst) {
	ref(Process) *procItem = inst.addr;

	printf("onDestroy(%i)\n", procItem->entry->ch->id);
	fflush(stdout);

	Process_Data *data = procItem->data;
	EventLoop_detach(EventLoop_GetInstance(), procItem->entry, true);
	Process_Data_destroy(data);
}

static sdef(void, onData, Instance inst) {
	ref(Process) *procItem = inst.addr;

	printf("onData fd=%i\n", procItem->entry->ch->id);
	fflush(stdout);

	String s = String_New(1024);
	s.len = Channel_Read(procItem->entry->ch, s.buf, String_GetSize(s));

	if (String_Equals(s.rd, $("Hello World\n"))) {
		System_out($("Received 'Hello World'.\n"));
	} else {
		System_out($("Received unexpected data."));
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

	assert(process == null);
	process = procItem;
}

def(void, exec) {
	Process proc = Process_new($("/bin/echo"));
	Process_setPipe(&proc, Process_Pipe_Read);
	Process_addParameter(&proc, $("Hello World"));

	Process_Data *data = Process_spawn(&proc);

	/* Listen for input on pipe. */
	call(listen, &proc, data);

	printf("pid=%i start\n", data->pid);
	fflush(stdout);

	Process_destroy(&proc);
}

def(void, exit, Signal_Type type) {
	System_out($("Early exit.\n"));

	if (process != null) {
		/* Process is still running. */
		System_out($("Kill process.\n"));
		EventLoop_enqueue(EventLoop_GetInstance(),
			process->entry, ChannelWatcher_Events_HangUp);
	}

	EventLoop_quit(EventLoop_GetInstance());
}

def(bool, Run) {
	Signal_listen(Signal_GetInstance());

	Signal_uponTermination(Signal_GetInstance(),
		Signal_OnTerminate_For(this, ref(exit)));

	call(exec);

	EventLoop_run(EventLoop_GetInstance());

	return true;
}
