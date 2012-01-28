/* Spawns one process and writes to its stdin. */

#import <Main.h>
#import <String.h>
#import <Process.h>

#define self Application

record(ref(Process)) {
	self *inst;
	EventLoop_Entry *entry;
	Process_Data *data;
};

static RdString buffer = $(
	"Hello World. "
	"Hello World. "
	"Hello World. "
	"Hello World. "
	"Hello World. "
	"Hello World. "
	"Hello World. "
	"Hello World. "
	"Hello World. "
	"Hello World. "
	"Hello World. "
	"Hello World. "
	"Hello World. "
	"Hello World.\n"
);

static ref(Process) *process = null;

static size_t totalWritten = 0;

sdef(void, onDone, Instance inst, pid_t pid, u8 status) {
	process = null;

	ref(Process) *procItem = inst.addr;

	if (status == Process_SpawningProcessFailed) {
		printf("pid=%i error\n", pid);
	} else {
		printf("pid=%i done status=%i\n", pid, status);
	}

	fflush(stdout);

	EventLoop_enqueue(EventLoop_GetInstance(),
		procItem->entry, ChannelWatcher_Events_HangUp);

	EventLoop_quit(EventLoop_GetInstance());
}

static sdef(void, onDestroy, Instance inst) {
	ref(Process) *procItem = inst.addr;

	fprintf(stderr, "onDestroy(%i)\n", procItem->entry->ch->id);
	fflush(stderr);

	Process_Data *data = procItem->data;
	EventLoop_detach(EventLoop_GetInstance(), procItem->entry, true);
	Process_Data_destroy(data);
}

static sdef(void, onOutput, Instance inst) {
	ref(Process) *procItem = inst.addr;

	size_t lines = 0;

	for (;;) {
		size_t len = Channel_Write(procItem->entry->ch, buffer);

		if (len == 0) {
			/* Kernel queue is full. */
			printf("Wrote %i lines.\n", lines);
			fflush(stdout);
			break;
		}

		lines++;
		totalWritten++;

		if (totalWritten >= 10000) {
			printf("Wrote a total of %i lines. Now terminating.\n",
				totalWritten);

			Process_terminate(procItem->data->pid);

			/* The process' termination is queued. Stop listening for
			 * further write requests.
			 */
			EventLoop_finalise(EventLoop_GetInstance(), procItem->entry);

			break;
		}
	}
}

static sdef(void, onInput, Instance inst) {
	/* This method shouldn't be called as our data stream is
	 * unidirectional, i.e., we only feed data.
	 */
	ref(Process) *procItem = inst.addr;

	printf("onInput fd=%i\n", procItem->entry->ch->id);
	fflush(stdout);
}

def(void, listen, Process *proc, Process_Data *data) {
	EventLoop_Entry *entry =
		EventLoop_createEntry(EventLoop_GetInstance(), this,
			sizeof(ref(Process)));

	ref(Process) *procItem = (void *) entry->data;

	procItem->entry = entry;
	procItem->data  = data;

	EventLoop_Options opts = {
		.ch            = &data->ch,
		.edgeTriggered = false,
		.events        = {
			.inst      = { .addr = procItem },
			.onDestroy = ref(onDestroy),
			.onInput   = ref(onInput),
			.onOutput  = ref(onOutput)
		}
	};

	EventLoop_attach(EventLoop_GetInstance(), entry, opts);

	Signal_uponChildTermination(Signal_GetInstance(), data->pid,
		Signal_OnChildTerminate_For(procItem, ref(onDone)));

	assert(process == null);
	process = procItem;
}

def(void, exec) {
	Process proc = Process_new($("./StdInLineCounter.exe"));
	Process_setPipe(&proc, Process_Pipe_Write);

	Process_Data *data = Process_spawn(&proc);

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
