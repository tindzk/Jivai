#define BitMask_ArgSet(arg, mask) \
	((arg) | (mask))

#define BitMask_Set(var, mask) \
	((var) |= (mask))

#define BitMask_ArgClear(arg, mask) \
	((arg) & (~(mask)))

#define BitMask_Clear(var, mask) \
	((var) &= (~(mask)))

#define BitMask_ArgFlip(arg, mask) \
	((arg) ^ (mask))

#define BitMask_Flip(var, mask) \
	((var) ^= (mask))

#define BitMask_Has(arg, mask) \
	(bool) ((arg) & (mask))
