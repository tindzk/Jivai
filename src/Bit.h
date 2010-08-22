#define Bit(pos) \
	(1 << (pos))

#define Bit_ArgSet(arg, pos) \
	((arg) | (1 << (pos)))

#define Bit_Set(var, pos) \
	((var) |= (1 << (pos)))

#define Bit_ArgClear(arg, pos) \
	((arg) & ~(1 << (pos)))

#define Bit_Clear(var, pos) \
	((var) &= ~(1 << (pos)))

#define Bit_ArgFlip(arg, pos) \
	((arg) ^ (1 << (pos)))

#define Bit_Flip(var, pos) \
	((var) ^= (1 << (pos)))

#define Bit_Has(arg, pos) \
	(bool) ((arg) & (1 << (pos)))
