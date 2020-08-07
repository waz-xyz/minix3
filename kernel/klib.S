#
# Chooses between the Armv7 and 386 versions of the low level kernel code.

#include <minix/config.h>

#if (CHIP == ARM)
#include "klib_armv7.S"
#elif (CHIP == INTEL)
#include "klib_x86.S"
#endif
