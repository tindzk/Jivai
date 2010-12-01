#import "SocketConnectionStream.h"

#define self SocketConnectionStream

def(bool, IsEof) {
	return false;
}

Impl(Stream) = {
	.read  = (void *) ref(Read),
	.write = (void *) ref(Write),
	.close = (void *) ref(Close),
	.isEof = (void *) ref(IsEof)
};
