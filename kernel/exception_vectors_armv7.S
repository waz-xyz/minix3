#include <minix/config.h>
#include <minix/const.h>
#include <minix/com.h>
#include "const.h"
#include "sconst.h"

	.arch armv7a
	.syntax unified
        
/*===========================================================================*
 *			exception vector table			     	     *
 *===========================================================================*/

	.section .text

	.global	exception_vector_start
	.global	exception_vector_end
	.balign	SMALL_PAGE_SIZE

exception_vector_start:

	ldr	pc, reset_handler_addr
	ldr	pc, undefined_instruction_handler_addr
	ldr	pc, supervisor_call_handler_addr
	ldr	pc, prefetch_abort_handler_addr
	ldr	pc, data_abort_handler_addr
	nop
	ldr	pc, irq_handler_addr
	ldr	pc, fast_irq_handler_addr

reset_handler_addr:			.word reset_handler
undefined_instruction_handler_addr:	.word undefined_instruction_handler
supervisor_call_handler_addr:		.word supervisor_call_handler
prefetch_abort_handler_addr:		.word prefetch_abort_handler
data_abort_handler_addr:		.word data_abort_handler
irq_handler_addr:			.word irq_handler
fast_irq_handler_addr:			.word fast_irq_handler

exception_vector_end:
