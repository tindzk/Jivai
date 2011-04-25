#import "Timer.h"

#define self Timer

rsdef(self, New, ClockType type) {
	assert(type == ClockType_Monotonic || type == ClockType_Realtime);

	int id = timerfd_create(type, TFD_NONBLOCK);

	if (id == -1) {
		throw(UnknownError);
	}

	return (self) {
		.ch = Channel_New(id, FileStatus_NonBlock)
	};
}

def(void, Destroy) {
	Channel_Destroy(&this->ch);
}

def(void, SetTimer, int sec) {
	assert(sec != 0);

	struct itimerspec value = {
		.it_interval.tv_sec = 0,
		.it_value.tv_sec    = sec
	};

	int ret = timerfd_settime(Channel_GetId(&this->ch), 0, &value, NULL);

	if (ret < 0) {
		throw(UnknownError);
	}

	this->interval = false;
}

def(void, SetInterval, int sec) {
	assert(sec != 0);

	struct itimerspec value = {
		.it_interval.tv_sec = sec,
		.it_value.tv_sec    = sec
	};

	int ret = timerfd_settime(Channel_GetId(&this->ch), 0, &value, NULL);

	if (ret < 0) {
		throw(UnknownError);
	}

	this->interval = true;
}

/* Returns the number of expirations that have occurred since the last Read()
 * call.
 */
rdef(u64, Read) {
	u64 dropped;
	Channel_Read(&this->ch, &dropped, sizeof(dropped));
	return dropped;
}

static sdef(void, OnInvoke, GenericInstance inst) {
	ref(Task) *task = inst.object;

	/* If false, the timer is not recurring. Thus, we have to detach it even
	 * though the callback might return true.
	 */
	bool interval = Timer_IsInterval(&task->timer);

	/* Don't merge this with callbackRet() because this won't be executed when
	 * no callback is set.
	 */
	u64 dropped = Timer_Read(&task->timer);

	bool keep = callbackRet(task->cb, false, dropped);

	if (!interval || !keep) {
		EventLoop_DetachChannel(EventLoop_GetInstance(), task->entry, true);
		task->entry = NULL;
	}
}

static sdef(void, DestroyTask, GenericInstance inst) {
	ref(Task) *task = inst.object;

	if (task->entry != NULL) {
		EventLoop_DetachChannel(EventLoop_GetInstance(), task->entry, false);
		task->entry = NULL;
	}

	Timer_Destroy(&task->timer);
}

def(Task *, AsTask, ref(OnTimer) onTimer) {
	Task *task = Task_New(sizeof(ref(Task)), ref(DestroyTask));

	ref(Task) *data = (void *) task->data;

	data->cb    = onTimer;
	data->timer = *this;
	data->entry = EventLoop_AddChannel(EventLoop_GetInstance(), this->ch,
		EventLoop_OnInput_For(data, ref(OnInvoke)));

	return task;
}
