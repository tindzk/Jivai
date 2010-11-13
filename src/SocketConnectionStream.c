#import "SocketConnectionStream.h"
#import "App.h"

def(bool, IsEof) {
	return false;
}

StreamInterface Impl(self) = {
	.read  = (void *) ref(Read),
	.write = (void *) ref(Write),
	.close = (void *) ref(Close),
	.isEof = (void *) ref(IsEof)
};
