#import "OO.h"
#import "Types.h"
#import "Macros.h"
#import "Exception.h"

#define self FPU

/* x87 FPU exception modes. */
set(ref(Exception)) {
	ref(Exception_Invalid)        = 0x01,
	ref(Exception_Denormal)       = 0x02,
	ref(Exception_DivisionByZero) = 0x04,
	ref(Exception_Overflow)       = 0x08,
	ref(Exception_Underflow)      = 0x10,
	ref(Exception_InexactResult)  = 0x20,
	ref(Exception_Mask)           = 0xff,
	ref(Exception_All)            =
		ref(Exception_Invalid)        |
		ref(Exception_Denormal)       |
		ref(Exception_DivisionByZero) |
		ref(Exception_Overflow)       |
		ref(Exception_Underflow)      |
		ref(Exception_InexactResult),

	/* Contrary to the x87 control word (CW), the SSE unit's control
	 * word (MXCSR) has the the exception mask shifted by 7.
	 */
	ref(Exception_SSEShift) = 7
};

/* x87 FPU rounding modes. */
set(ref(Round)) {
	ref(Round_ToNearest) = 0x0000, /* Round to nearest (even).    */
	ref(Round_Truncate)  = 0x0001, /* Round towards 0 (truncate). */
	ref(Round_Upward)    = 0x0002, /* Round up towards +Inf.      */
	ref(Round_Downward)  = 0x0003, /* Round down towards -Inf.    */
	ref(Round_Mask)      = 0xc00,
	ref(Round_All)       =
		ref(Round_ToNearest) |
		ref(Round_Truncate)  |
		ref(Round_Downward)  |
		ref(Round_Upward),

	/* MXCSR has the rounding control bits shifted by 3. */
	ref(Round_SSEShift) = 3
};

sdef(void, setExceptions, int mode);
sdef(void, setRounding, int mode);

#undef self
