#import "FPU.h"

#define self FPU

/* Gets the current control word (CW) of the x87 FPU. */
static alwaysInline sdef(void, getX87ControlState, u16 *res) {
	/* Store the FPU control word without checking for pending unmasked
	 * floating-point exceptions.
	 */
	__asm__("fnstcw %0" : "=m" (*res));
}

/* Loads a x87 FPU control word (CW). */
static alwaysInline sdef(void, setX87ControlState, u16 state) {
	/* Load the 16-bit word located at the specified memory address
	 * into the control word register.
	 */
	__asm__("fldcw %0" : : "m" (state));
}

static alwaysInline sdef(void, getSSEControlState, u32 *res) {
	/* Store the contents of the Streaming SIMD Extension (SSE)
	 * control and status register in the specified memory location.
	 */
	__asm__("stmxcsr %0" : "=m" (*res));
}

static alwaysInline sdef(void, setSSEControlState, u32 state) {
	/* Load Streaming SIMD Extension (SSE) control and status
	 * register from memory.
	 */
	__asm__("ldmxcsr %0" : : "m" (state));
}

sdef(void, setExceptions, int mode) {
	assert((mode & ~ref(Exception_Mask)) == 0);

	u16 x87;
	scall(getX87ControlState, &x87);
	x87 &= ~mode;
	scall(setX87ControlState, x87);

#if defined(__SSE__)
	u32 sse;
	scall(getSSEControlState, &sse);
	sse &= ~(mode << ref(Exception_SSEShift));
	scall(setSSEControlState, sse);
#endif
}

sdef(void, setRounding, int mode) {
	assert((mode & ~ref(Round_Mask)) == 0);

	u16 control;
	scall(getX87ControlState, &control);
	control &= ~ref(Round_Mask);
	control |= mode;
	scall(setX87ControlState, control);

#if defined(__SSE__)
	u32 sse;
	scall(getSSEControlState, &sse);
	sse &= ~(ref(Round_Mask) << ref(Round_SSEShift));
	sse |= mode << ref(Round_SSEShift);
	scall(setSSEControlState, sse);
#endif
}
