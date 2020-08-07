#include <minix/config.h>
#include <minix/const.h>
#include "const.h"

	.arch armv7a
	.syntax unified

	.section .text

	.global idle_task
	.type	idle_task, %function

	.global intr_disable
	.type	intr_disable, %function
	
	.global intr_enable
	.type	intr_enable, %function

	.global enable_irq
	.type	enable_irq, %function

	.global disable_irq
	.type	disable_irq, %function

	.global cp_mess
	.type	cp_mess, %function

	.global phys_copy
	.type	phys_copy, %function

	.global phys_memset
	.type	phys_memset, %function

intr_disable:
	bx      lr

intr_enable:
	bx      lr

/*===========================================================================*
 *				idle_task				     *
 *===========================================================================*
 * This task is called when the system has nothing else to do.  The HLT
 * instruction puts the processor in a state where it draws minimum power.
 */
idle_task:
	bx	lr

/*==========================================================================*
 *				enable_irq				    *
 *==========================================================================*
 * PUBLIC void enable_irq(irq_hook_t *hook)
 * Enable an interrupt request line by clearing an 8259 bit.
 * Equivalent C code for hook->irq < 8:
 *   if ((irq_actids[hook->irq] &= ~hook->id) == 0)
 *	outb(INT_CTLMASK, inb(INT_CTLMASK) & ~(1 << irq));
 */

enable_irq:
	bx      lr

/*==========================================================================*
 *				disable_irq				    *
 *==========================================================================*
 * PUBLIC int disable_irq(irq_hook_t *hook)
 * Disable an interrupt request line by setting an 8259 bit.
 * Equivalent C code for irq < 8:
 *   irq_actids[hook->irq] |= hook->id;
 *   outb(INT_CTLMASK, inb(INT_CTLMASK) | (1 << irq));
 * Returns true iff the interrupt was not already disabled.
 */

disable_irq:
	bx	lr

/*===========================================================================*
 *				cp_mess					     *
 *===========================================================================*
 * PUBLIC void cp_mess(int src, phys_clicks src_clicks, vir_bytes src_offset,
 *		      phys_clicks dst_clicks, vir_bytes dst_offset);
 * This routine makes a fast copy of a message from anywhere in the address
 * space to anywhere else.  It also copies the source address provided as a
 * parameter to the call into the first word of the destination message.
 *
 * Note that the message size, "Msize" is in DWORDS (not bytes) and must be set
 * correctly.  Changing the definition of message in the type file and not
 * changing it here will lead to total disaster.
 */

cp_mess:
	bx      lr

/*===========================================================================*
 *				phys_copy				     *
 *===========================================================================*
 * PUBLIC void phys_copy(phys_bytes source, phys_bytes destination,
 *			phys_bytes bytecount);
 * Copy a block of physical memory.
 */

phys_copy:
	bx	lr

/*===========================================================================*
 *				phys_memset				     *
 *===========================================================================*
 * PUBLIC void phys_memset(phys_bytes source, unsigned long pattern,
 *	phys_bytes bytecount);
 * Fill a block of physical memory with pattern.
 */

phys_memset:
	bx	lr