#import "SocketConnectionStream.h"

#define self SocketConnectionStream

def(bool, IsEof) {
	return false;
}

Impl(Stream) = {
	.read  = ref(Read),
	.write = ref(Write),
	.close = ref(Close),
	.isEof = ref(IsEof)
};
