#include <minix/config.h>
#include <minix/const.h>
#include "const.h"
#include "sconst.h"

	.arch armv7a
	.syntax unified

	.section .text

	.global	halt_cpu
	.type	halt_cpu, %function

	.global idle_task
	.type	idle_task, %function

	.global intr_disable
	.type	intr_disable, %function
	
	.global intr_enable
	.type	intr_enable, %function

	.global read_psr
	.type	read_psr, %function

	.global read_system_register
	.type	read_system_register, %function
	
	.global invalidate_system_structures
	.type	invalidate_system_structures, %function

/*===========================================================================*
 *				intr_disable				     *
 *===========================================================================*
 * Disables all sources of interrupts (IRQ and FIRQ).
 */
intr_disable:
	cpsid	if
	bx      lr

/*===========================================================================*
 *				intr_disable				     *
 *===========================================================================*
 * Enables all sources of interrupts (IRQ and FIRQ).
 */
intr_enable:
	cpsie	if
	bx      lr

/*===========================================================================*
 *				halt_cpu				     *
 *===========================================================================*
 * This function is called when the system has nothing else to do.
 */
halt_cpu:
	cpsid	if
	wfe
	b	.

/*===========================================================================*
 *				idle_task				     *
 *===========================================================================*
 * This task is called when the system has nothing else to do.  The HLT
 * instruction puts the processor in a state where it draws minimum power.
 * It must be Thumb code, because all kernel tasks use this instruction set.
 */
	.thumb_func
idle_task:
	cpsie	if
	wfe
	bx	lr

	.arm
	.align	2

/*===========================================================================*
 *			      read_psr					     *
 *===========================================================================*
 * PUBLIC uint32_t read_psr(int type);
 * Read *PSR from C.
 */
read_psr:
	cmp	r0, READ_APSR
	mrseq	r0, apsr
	beq	0f
	cmp	r0, READ_CPSR
	mrseq	r0, cpsr
	beq	0f
	cmp	r0, READ_SPSR
	mrseq	r0, spsr
	movne	r0, 0
0:
	bx	lr

/*===========================================================================*
 *			      read_system_register			     *
 *===========================================================================*
 * PUBLIC uint32_t read_system_register(int type);
 * Read a system register from C.
 */
read_system_register:
	cmp	r0, READ_SCTLR
	mrceq	p15, 0, r0, c1, c0, 0
	beq	0f
	cmp	r0, READ_TTBR0
	mrceq	p15, 0, r0, c2, c0, 0
	beq	0f
	cmp	r0, READ_TTBR1
	mrceq	p15, 0, r0, c2, c0, 1
	beq	0f
	cmp	r0, READ_TTBCR
	mrceq	p15, 0, r0, c2, c0, 2
	beq	0f
	cmp	r0, READ_IFSR
	mrceq	p15, 0, r0, c5, c0, 1
	beq	0f
	cmp	r0, READ_IFAR
	mrceq	p15, 0, r0, c6, c0, 2
	beq	0f
	cmp	r0, READ_DFSR
	mrceq	p15, 0, r0, c5, c0, 0
	beq	0f
	cmp	r0, READ_DFAR
	mrceq	p15, 0, r0, c6, c0, 0
	movne	r0, 0
0:	bx	lr

/*===========================================================================*
 *			      invalidate_system_structures		     *
 *===========================================================================*
 * PUBLIC void invalidate_system_structures(int type);
 */
invalidate_system_structures:
	cmp	r0, INVALIDATE_TLB
	mcreq	p15, 0, r0, c8, c7, 0		// Execute TLBIALL
	beq	0f
	cmp	r0, INVALIDATE_ICACHE
	mcreq	p15, 0, r0, c7, c5, 0		// Execute ICIALLU
	beq	0f
	cmp	r0, INVALIDATE_BPREDICTOR
	mcreq	p15, 0, r0, c7, c5, 6		// Execute BPIALL
0:	dsb
	isb					// Make sure the call has an effect
	bx	lr


	.global set_leds
	.type	set_leds, %function
set_leds:
	ldr	r1, =#0x80022000
	str	r0, [r1]
	bx	lr
