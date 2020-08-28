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

	uint32_t cpsr = read_psr(READ_CPSR);
	uint32_t sctlr = read_system_register(READ_SCTLR);
	kprintf("CPSR = 0x%08X\nSCTLR = 0x%08X\n", cpsr, sctlr);
	/* Set exception vector table at physical address 0 */
	char *ex_start = &exception_vector_start, *ex_end = &exception_vector_end;
	kprintf("exception_vector_start = 0x%08X\nexception_vector_end = 0x%08X\n", ex_start, ex_end);
}

/*===========================================================================*
 *				put_irq_handler				     *
 *===========================================================================*/
PUBLIC void put_irq_handler(irq_hook_t *hook, int irq, irq_handler_t handler)
/* Register an interrupt handler. */
{
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
/* Unregister an interrupt handler. */
{
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
/* Call the interrupt handlers for an interrupt with the given hook list.
 * The assembly part of the handler has already masked the IRQ, reenabled the
 * controller(s) and enabled interrupts.
 */
PUBLIC void intr_handle(irq_hook_t *hook)
{
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

/*===========================================================================*
 *			generic_interrupt_handler		     	     *
 *===========================================================================*/
PUBLIC void generic_interrupt_handler(void)
{
	//struct intc_csreg *csrptr = (struct intc_csreg *)0x48200000;
	irq_hook_t *hook;
	int irqnum;

	irqnum = 0;
	hook = irq_handlers[irqnum];
	intr_handle(hook);
	if (irq_actids[irqnum] != 0)
	{
		;
	}
}


/*===========================================================================*
 *				get_irq_num				     *
 *===========================================================================*/
PRIVATE int get_irq_num(void)
{
	
}