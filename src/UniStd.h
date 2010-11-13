/* Required for Clang when the code is compiled with -fblocks
 * because this function definition is incompatible then:
 *   extern void encrypt (char *__block, int __edflag) __THROW __nonnull ((1));
 * Clang reports:
 *   error: __block attribute not allowed, only allowed on local variables
 */

#undef __USE_XOPEN
#import <unistd.h>
#define __USE_XOPEN
