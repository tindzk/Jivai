#import "Types.h"

Interface(Stream) {
	size_t (*read) (Instance $this, void *buf, size_t len);
	size_t (*write)(Instance $this, void *buf, size_t len);
	void   (*close)(Instance $this);
	bool   (*isEof)(Instance $this);
};
