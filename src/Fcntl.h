typedef enum {
	FcntlMode_GetDescriptorFlags = 1,
	FcntlMode_SetDescriptorFlags = 2,
	FcntlMode_GetStatus          = 3,
	FcntlMode_SetStatus          = 4
} FcntlMode;

typedef enum {
	FileDescriptorFlags_CloseOnExec = 1
} FileDescriptorFlags;
