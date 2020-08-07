/* This file contains routines for initializing the 8259 interrupt controller:
 *	put_irq_handler: register an interrupt handler
 *	rm_irq_handler: deregister an interrupt handler
 *	intr_handle:	handle a hardware interrupt
 *	intr_init:	initialize the interrupt controller(s)
 */

#include "kernel.h"
#include "proc.h"
#include <minix/com.h>

int __aeabi_idiv0(int return_value)
{
	return 0;
}

long long __aeabi_ldiv0(long long return_value)
{
	return 0;
}

/*===========================================================================*
 *				intr_init				     *
 *===========================================================================*/
void intr_init(int mine)
{
	(void)mine;
}

/*===========================================================================*
 *				put_irq_handler				     *
 *===========================================================================*/
void put_irq_handler(irq_hook_t *hook, int irq, irq_handler_t handler)
{

}

/*===========================================================================*
 *				rm_irq_handler				     *
 *===========================================================================*/
void rm_irq_handler(irq_hook_t *hook)
{

}