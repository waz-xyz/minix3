#include <minix/config.h>
#include <minix/const.h>
#include <minix/com.h>
#include "const.h"

	.arch armv7a
	.syntax unified

	.section .text

	.global restart
	.type	restart, %function

/*===========================================================================*
 *				restart					     *
 *===========================================================================*
 * Restart the current process or the next process if it is set. 
 */
restart:
        bx      lr
