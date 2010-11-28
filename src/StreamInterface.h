#import "Types.h"

Interface(Stream) {
	size_t (*read) (GenericInstance, void *, size_t);
	size_t (*write)(GenericInstance, void *, size_t);
	void   (*close)(GenericInstance);
	bool   (*isEof)(GenericInstance);
};
