#import "String.h"
#import "Kernel.h"
#import "Exception.h"
#import "SocketConnection.h"

#define self Socket

exc(SetSocketOption)
exc(SocketFailed)

set(ref(Protocol)) {
	ref(Protocol_TCP),
	ref(Protocol_UDP)
};

set(ref(Option)) {
	ref(Option_Blocking)    = Bit(0),
	ref(Option_CloseOnExec) = Bit(1)
};

class {
	Channel ch;
	Socket_Protocol protocol;
};

rsdef(self, New, ref(Protocol) protocol, int options);
def(void, Destroy);
def(void, SetReusable, bool enable);
def(void, SetLinger);

static alwaysInline def(Channel *, GetChannel) {
	return &this->ch;
}

static alwaysInline def(ref(Protocol), GetProtocol) {
	return this->protocol;
}

#undef self
