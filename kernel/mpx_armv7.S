#include <minix/config.h>
#include <minix/const.h>
#include <minix/com.h>
#include "const.h"

	.arch armv7a
	.syntax unified

	.section .text.boot

	.global _start
	.type	_start, %function

/*===========================================================================*
 *				MINIX					     *
 *===========================================================================*/
_start:			/* this is the entry point for the MINIX kernel */
	ldr	pc, =cstart

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
