/* This file contains routines for initializing the 8259 interrupt controller:
 *	put_irq_handler: register an interrupt handler
 *	rm_irq_handler: deregister an interrupt handler
 *	intr_handle:	handle a hardware interrupt
 *	intr_init:	initialize the interrupt controller(s)
 */

#include "kernel.h"
#include "proc.h"
#include <minix/com.h>

/*===========================================================================*
 *				intr_init				     *
 *===========================================================================*/
PUBLIC void intr_init(int mine)
{
	(void)mine;

	intr_disable();

	/* Set exception vector table at physical address 0 */
	copy_vir2phys(exception_vector_start, 0, exception_vector_end-exception_vector_start);
}

/*===========================================================================*
 *				put_irq_handler				     *
 *===========================================================================*/
PUBLIC void put_irq_handler(irq_hook_t *hook, int irq, irq_handler_t handler)
{
	/* Register an interrupt handler. */
	int id;
	irq_hook_t **line;

	if (irq < 0 || irq >= NR_IRQ_VECTORS)
		panic("invalid call to put_irq_handler", irq);

	line = &irq_handlers[irq];
	id = 1;
	while (*line != NULL)
	{
		if (hook == *line) return;	/* extra initialization */
		line = &(*line)->next;
		id <<= 1;
	}
	if (id == 0)
		panic("Too many handlers for irq", irq);

	hook->next = NULL;
	hook->handler = handler;
	hook->irq = irq;
	hook->id = id;
	*line = hook;

	irq_use |= 1 << irq;
}

/*===========================================================================*
 *				rm_irq_handler				     *
 *===========================================================================*/
PUBLIC void rm_irq_handler(irq_hook_t *hook)
{
	/* Unregister an interrupt handler. */
	int irq = hook->irq; 
	int id = hook->id;
	irq_hook_t **line;

	if (irq < 0 || irq >= NR_IRQ_VECTORS) 
		panic("invalid call to rm_irq_handler", irq);

	line = &irq_handlers[irq];
	while (*line != NULL)
	{
		if ((*line)->id == id)
		{
			(*line) = (*line)->next;
			if (!irq_handlers[irq])
				irq_use &= ~(1 << irq);
			return;
		}
		line = &(*line)->next;
	}
	/* When the handler is not found, normally return here. */
}

/*===========================================================================*
 *				intr_handle				     *
 *===========================================================================*/
PUBLIC void intr_handle(irq_hook_t *hook)
{
	/* Call the interrupt handlers for an interrupt with the given hook list.
	 * The assembly part of the handler has already masked the IRQ, reenabled the
	 * controller(s) and enabled interrupts.
	 */

	/* Call list of handlers for an IRQ. */
	while (hook != NULL)
	{
		/* For each handler in the list, mark it active by setting its ID bit,
		* call the function, and unmark it if the function returns true.
		*/
		irq_actids[hook->irq] |= hook->id;
		if ((*hook->handler)(hook))
			irq_actids[hook->irq] &= ~hook->id;
		hook = hook->next;
	}

	/* The assembly code will now disable interrupts, unmask the IRQ if and only
	* if all active ID bits are cleared, and restart a process.
	*/
}
