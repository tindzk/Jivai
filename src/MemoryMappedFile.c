#import "MemoryMappedFile.h"

#define self MemoryMappedFile

rsdef(self, new, RdString path) {
	File file = File_New(path, FileStatus_ReadOnly);
	Stat64 sb = File_GetStat(&file);

	/* Must be a regular file to be mmap()-able. */
	if ((sb.mode & FileMode_Regular) == 0) {
		throw(InvalidFile);
	}

	self res = {
		.addr = mmap(0, sb.size, PROT_READ, MAP_SHARED, file.ch.id, 0),
		.size = sb.size
	};

	if (res.addr == MAP_FAILED) {
		throw(UnknownError);
	}

	File_Destroy(&file);

	return res;
}

def(void, destroy) {
	if (munmap(this->addr, this->size) == -1) {
		throw(UnknownError);
	}
}
