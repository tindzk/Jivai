#import "CPU.h"

#define self CPU

rsdef(u8, getCores) {
	u32 ebx;

	asm("cpuid" : "=b" (ebx) : "a" (1));

	u8 *arr = (u8 *) &ebx;

	return (arr[2] != 0)
		? arr[2]
		: 1;
}
