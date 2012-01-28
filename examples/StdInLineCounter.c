/* Implementation of wc -l. Contrary to the GNU implementation, this
 * won't crash on EAGAIN.
 */

#import <Main.h>
#import <String.h>

#define self Application

static size_t lines = 0;

record(ref(Context)) {
	EventLoop_Entry *entry;
};

static sdef(void, onDestroy, Instance inst) {
	ref(Context) *context = inst.addr;

	fprintf(stderr, "onDestroy(%i)\n", context->entry->ch->id);
	fflush(stderr);

	EventLoop_detach(EventLoop_GetInstance(), context->entry, true);
}

static sdef(void, onInput, Instance inst) {
	ref(Context) *context = inst.addr;

	String s = String_New(1024);

	for (;;) {
		s.len = Channel_Read(context->entry->ch, s.buf, String_GetSize(s));

		if (s.len == 0) {
			break;
		}

		fwd(i, s.len) {
			if (s.buf[i] == '\n') {
				lines++;
			}
		}
	}

	String_Destroy(&s);
}

def(void, listen) {
	EventLoop_Entry *entry =
		EventLoop_createEntry(EventLoop_GetInstance(), this,
			sizeof(ref(Context)));

	ref(Context) *context = (void *) entry->data;

	context->entry = entry;

	EventLoop_Options opts = {
		.ch            = Channel_StdIn,
		.edgeTriggered = false,
		.events        = {
			.inst      = { .addr = context },
			.onDestroy = ref(onDestroy),
			.onInput   = ref(onInput)
		}
	};

	EventLoop_attach(EventLoop_GetInstance(), entry, opts);
}

def(void, exit, Signal_Type type) {
	System_err($("Early exit.\n"));
	EventLoop_quit(EventLoop_GetInstance());
}

def(bool, Run) {
	Signal_listen(Signal_GetInstance());

	Signal_uponTermination(Signal_GetInstance(),
		Signal_OnTerminate_For(this, ref(exit)));

	call(listen);

	EventLoop_run(EventLoop_GetInstance());

	fprintf(stderr, "%i\n", lines);

	return true;
}
