#import "Types.h"
#import "Buffer.h"

Interface(Stream) {
	size_t (*read) (Instance $this, WrBuffer buf);
	size_t (*write)(Instance $this, RdBuffer buf);
	void   (*close)(Instance $this);
	bool   (*isEof)(Instance $this);
};
