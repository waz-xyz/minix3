#include <minix/config.h>
#include <minix/const.h>
#include <minix/com.h>
#include "const.h"
#include "sconst.h"

	.arch armv7a
	.syntax unified
        
/*===========================================================================*
 *				exception handlers		     	     *
 *===========================================================================*/

	.global	reset_handler
	.global	undefined_instruction_handler
	.global	supervisor_call_handler
	.global	prefetch_abort_handler
	.global	data_abort_handler
	.global	irq_handler
	.global	fast_irq_handler

	.type	reset_handler, %function
	.type	undefined_instruction_handler, %function
	.type	supervisor_call_handler, %function
	.type	prefetch_abort_handler, %function
	.type	data_abort_handler, %function
	.type	irq_handler, %function
	.type	fast_irq_handler, %function

reset_handler:
	srsdb	sp!, #MODE_SVC
	cps	#MODE_SVC
	bl	save
	mov	r0, RESET_EXCEPTION
	b	generic_exception_handler

undefined_instruction_handler:
	srsdb	sp!, #MODE_SVC
	cps	#MODE_SVC
	bl	save
	mov	r0, UNDEFINED_INSTRUCTION_EXCEPTION
	b	generic_exception_handler

supervisor_call_handler:
	srsdb	sp!, #MODE_SVC
	bl	save
	mov	r0, SUPERVISOR_CALL_EXCEPTION
	b	generic_exception_handler

prefetch_abort_handler:
	sub	lr, #4
	srsdb	sp!, #MODE_SVC
	cps	#MODE_SVC
	bl	save
	mov	r0, PREFETCH_ABORT_EXCEPTION
	b	generic_exception_handler

data_abort_handler:
	sub	lr, #8
	srsdb	sp!, #MODE_SVC
	cps	#MODE_SVC
	bl	save
	mov	r0, DATA_ABORT_EXCEPTION
	b	generic_exception_handler

irq_handler:
	sub	lr, #4
	srsdb	sp!, #MODE_SVC
	cps	#MODE_SVC
	bl	save
	mov	r0, IRQ_EXCEPTION
	b	generic_exception_handler

fast_irq_handler:
	sub	lr, #4
	srsdb	sp!, #MODE_SVC
	cps	#MODE_SVC
	bl	save
	mov	r0, FAST_IRQ_EXCEPTION
	// Fall into generic exception handling

generic_exception_handler:
	// k_reenter += 1; keep the new value at r4 (callee-saved)
	ldr	r1, =k_reenter
	ldr	r4, [r1]
	add	r4, 1
	str	r4, [r1]
	// The stack must be aligned at 8 bytes by ARM ABI requirements.
	// Keep the offset at r5 (callee-saved).
	and	r5, sp, #4
	sub	sp, r5
	ldr	r1, =exception
	blx	r1
	add	sp, r5
	// If ++k_reenter != 0 we are reentering the kernel
	cmp	r4, 0
	beq	restart
	bne	restart1

// TODO: Make it useful
generic_interrupt_handler:
	push	{r0-r4, r12, lr}
	and	r4, sp, #4
	sub	sp, r4
	ldr	lr, =intr_handle
	blx	lr
	add	sp, r4
	pop	{r0-r4, r12, lr}
	rfe	sp!
