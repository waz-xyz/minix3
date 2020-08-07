#
# Chooses between the Armv7 and 386 versions of the Minix startup code.

#include <minix/config.h>

#if (CHIP == ARM)
#include "mpx_armv7.S"
#elif (CHIP == INTEL)
#include "mpx_x86.S"
#endif
