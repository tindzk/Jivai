#import "Socket.h"

#define self Socket

rsdef(self, New, ref(Protocol) protocol, int options) {
	int type = (protocol == ref(Protocol_TCP))
		? SOCK_STREAM
		: SOCK_DGRAM;

	if (!BitMask_Has(options, ref(Option_Blocking))) {
		BitMask_Set(type, SOCK_NONBLOCK);
	}

	if (BitMask_Has(options, ref(Option_CloseOnExec))) {
		BitMask_Set(type, SOCK_CLOEXEC);
	}

	int proto = (protocol == ref(Protocol_TCP))
		? IPPROTO_TCP
		: IPPROTO_UDP;

	int id = Kernel_socket(PF_INET, type, proto);

	if (id == -1) {
		throw(SocketFailed);
	}

	int flags = !BitMask_Has(options, ref(Option_Blocking))
		? FileStatus_NonBlock
		: 0;

	return (self) {
		.ch       = Channel_New(id, flags),
		.protocol = protocol
	};
}

def(void, Destroy) {
	Kernel_shutdown(Channel_GetId(&this->ch), SHUT_RDWR);
	Channel_Destroy(&this->ch);
}

def(void, SetLinger) {
	struct linger ling = {
		.l_onoff  = 1,
		.l_linger = 30
	};

	if (!Kernel_setsockopt(Channel_GetId(&this->ch), SOL_SOCKET, SO_LINGER, &ling, sizeof(ling))) {
		throw(SetSocketOption);
	}
}
