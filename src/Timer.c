#import "Timer.h"

#define self Timer

rsdef(self, new, ClockType type) {
	assert(type == ClockType_Monotonic
		|| type == ClockType_Realtime);

	int id = timerfd_create(type, TFD_NONBLOCK);

	if (id == -1) {
		throw(UnknownError);
	}

	return (self) {
		.ch = Channel_New(id, FileStatus_NonBlock)
	};
}

def(void, destroy) {
	Channel_Destroy(&this->ch);
}

/* Timer expiring in `start' seconds from now and then repeated every
 * `interval' seconds.
 */
overload def(void, setRelative, time_t start, time_t interval) {
	assert(start != 0);

	struct itimerspec value = {
		.it_interval.tv_sec = interval,
		.it_value.tv_sec    = start
	};

	int ret = timerfd_settime(Channel_GetId(&this->ch), 0, &value, null);

	if (ret < 0) {
		throw(UnknownError);
	}

	this->interval = (interval != 0);
}

overload def(void, setAbsolute, time_t start, time_t interval) {
	assert(start != 0);

	struct itimerspec value = {
		.it_interval.tv_sec = interval,
		.it_value.tv_sec    = start
	};

	int ret = timerfd_settime(
		Channel_GetId(&this->ch), TFD_TIMER_ABSTIME, &value, null);

	if (ret < 0) {
		throw(UnknownError);
	}

	this->interval = (interval != 0);
}

/* Returns the number of expirations that have occurred since the last
 * read() call.
 */
rdef(u64, read) {
	u64 dropped;
	Channel_Read(&this->ch, &dropped, sizeof(dropped));
	return dropped;
}

static sdef(void, onInvoke, Instance inst) {
	ref(Task) *task = inst.addr;

	/* If false, the timer is not recurring. Thus, we have to detach it
	 * even though the callback might return true.
	 */
	bool interval = scall(isInterval, &task->timer);

	/* Don't merge this with callbackRet() because this won't be
	 * executed when no callback is set.
	 */
	u64 dropped = scall(read, &task->timer);

	bool keep = callbackRet(task->cb, false, dropped);

	if (!interval || !keep) {
		EventLoop_detach(EventLoop_GetInstance(), task->entry, true);
	}
}

static sdef(void, destroyTask, Instance inst) {
	ref(Task) *task = inst.addr;

	scall(destroy, &task->timer);

	if (task->entry != null) {
		EventLoop_detach(EventLoop_GetInstance(), task->entry, false);
	}
}

def(Task *, asTask, ref(OnTimer) onTimer) {
	EventLoop_Entry *entry = EventLoop_createEntry(
		EventLoop_GetInstance(), this, sizeof(ref(Task)));

	ref(Task) *task = (void *) entry->data;

	task->cb    = onTimer;
	task->timer = *this;
	task->entry = entry;
	task->task  = Task_new(task, ref(destroyTask));

	EventLoop_Options opts = {
		.ch     = &task->timer.ch,
		.events = {
			.inst      = { .addr = task },
			.onDestroy = ref(destroyTask),
			.onInput   = ref(onInvoke)
		}
	};

	EventLoop_attach(EventLoop_GetInstance(), entry, opts);

	return task->task;
}
