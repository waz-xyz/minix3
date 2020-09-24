/* This file contains routines for initializing the 8259 interrupt controller:
 *	put_irq_handler: register an interrupt handler
 *	rm_irq_handler: deregister an interrupt handler
 *	intr_handle:	handle a hardware interrupt
 *	intr_init:	initialize the interrupt controller(s)
 */

#include "kernel.h"
#include "proc.h"
#include <minix/com.h>

#define	ICC_BASE	0xF8F00100
#define	ICCICR		(ICC_BASE + 0x00)
#define	ICCPMR		(ICC_BASE + 0x04)
#define	ICCIAR		(ICC_BASE + 0x0C)
#define	ICCEOIR		(ICC_BASE + 0x10)
#define	ICCIIDR		(ICC_BASE + 0xFC)

#define	ICD_BASE	0xF8F01000
#define	ICDDCR		(ICD_BASE + 0x000)
#define	ICDICTR		(ICD_BASE + 0x004)
#define	ICDISER0	(ICD_BASE + 0x100)
#define	ICDICER0	(ICD_BASE + 0x180)
#define	ICDIPR0		(ICD_BASE + 0x400)
#define	ICDIPTR0	(ICD_BASE + 0x800)
#define	ICDICFR0	(ICD_BASE + 0xC00)
#define	ICDSGIR		(ICD_BASE + 0xF00)

/* Sensitivities for each possible interrupt source
 * Only PL[15:0] may be changed at any time.
 */
int sensitivities[] =
{
	[IRQ_SGI0]		= GIC_SGI_EDGE_SENSITIVE,
	[IRQ_SGI1]		= GIC_SGI_EDGE_SENSITIVE,
	[IRQ_SGI2]		= GIC_SGI_EDGE_SENSITIVE,
	[IRQ_SGI3]		= GIC_SGI_EDGE_SENSITIVE,
	[IRQ_SGI4]		= GIC_SGI_EDGE_SENSITIVE,
	[IRQ_SGI5]		= GIC_SGI_EDGE_SENSITIVE,
	[IRQ_SGI6]		= GIC_SGI_EDGE_SENSITIVE,
	[IRQ_SGI7]		= GIC_SGI_EDGE_SENSITIVE,
	[IRQ_SGI8]		= GIC_SGI_EDGE_SENSITIVE,
	[IRQ_SGI9]		= GIC_SGI_EDGE_SENSITIVE,
	[IRQ_SGI10]		= GIC_SGI_EDGE_SENSITIVE,
	[IRQ_SGI11]		= GIC_SGI_EDGE_SENSITIVE,
	[IRQ_SGI12]		= GIC_SGI_EDGE_SENSITIVE,
	[IRQ_SGI13]		= GIC_SGI_EDGE_SENSITIVE,
	[IRQ_SGI14]		= GIC_SGI_EDGE_SENSITIVE,
	[IRQ_SGI15]		= GIC_SGI_EDGE_SENSITIVE,
	[IRQ_GLOBAL_TIMER]	= GIC_EDGE_SENSITIVE,
	[IRQ_NFIQ]		= GIC_LEVEL_SENSITIVE,
	[IRQ_PRIVATE_TIMER]	= GIC_EDGE_SENSITIVE,
	[IRQ_AWDT]		= GIC_EDGE_SENSITIVE,
	[IRQ_NIRQ]		= GIC_LEVEL_SENSITIVE,
	[IRQ_CPU0]		= GIC_EDGE_SENSITIVE,
	[IRQ_CPU1]		= GIC_EDGE_SENSITIVE,
	[IRQ_L2CACHE]		= GIC_LEVEL_SENSITIVE,
	[IRQ_OCM]		= GIC_LEVEL_SENSITIVE,
	[IRQ_PMU0]		= GIC_LEVEL_SENSITIVE,
	[IRQ_PMU1]		= GIC_LEVEL_SENSITIVE,
	[IRQ_XADC]		= GIC_LEVEL_SENSITIVE,
	[IRQ_DEVC]		= GIC_LEVEL_SENSITIVE,
	[IRQ_SWDT]		= GIC_EDGE_SENSITIVE,
	[IRQ_TTC0_0]		= GIC_LEVEL_SENSITIVE,
	[IRQ_TTC0_1]		= GIC_LEVEL_SENSITIVE,
	[IRQ_TTC0_2]		= GIC_LEVEL_SENSITIVE,
	[IRQ_DMAC_ABORT]	= GIC_LEVEL_SENSITIVE,
	[IRQ_DMAC0]		= GIC_LEVEL_SENSITIVE,
	[IRQ_DMAC1]		= GIC_LEVEL_SENSITIVE,
	[IRQ_DMAC2]		= GIC_LEVEL_SENSITIVE,
	[IRQ_DMAC3]		= GIC_LEVEL_SENSITIVE,
	[IRQ_SMC]		= GIC_LEVEL_SENSITIVE,
	[IRQ_QSPI]		= GIC_LEVEL_SENSITIVE,
	[IRQ_GPIO]		= GIC_LEVEL_SENSITIVE,
	[IRQ_USB0]		= GIC_LEVEL_SENSITIVE,
	[IRQ_ETHERNET0]		= GIC_LEVEL_SENSITIVE,
	[IRQ_ETHERNET0_WAKEUP]	= GIC_EDGE_SENSITIVE,
	[IRQ_SDIO0]		= GIC_LEVEL_SENSITIVE,
	[IRQ_I2C0]		= GIC_LEVEL_SENSITIVE,
	[IRQ_SPI0]		= GIC_LEVEL_SENSITIVE,
	[IRQ_UART0]		= GIC_LEVEL_SENSITIVE,
	[IRQ_CAN0]		= GIC_LEVEL_SENSITIVE,
	[IRQ_PL0]		= GIC_EDGE_SENSITIVE,
	[IRQ_PL1]		= GIC_EDGE_SENSITIVE,
	[IRQ_PL2]		= GIC_EDGE_SENSITIVE,
	[IRQ_PL3]		= GIC_EDGE_SENSITIVE,
	[IRQ_PL4]		= GIC_EDGE_SENSITIVE,
	[IRQ_PL5]		= GIC_EDGE_SENSITIVE,
	[IRQ_PL6]		= GIC_EDGE_SENSITIVE,
	[IRQ_PL7]		= GIC_EDGE_SENSITIVE,
	[IRQ_TTC1_0]		= GIC_LEVEL_SENSITIVE,
	[IRQ_TTC1_1]		= GIC_LEVEL_SENSITIVE,
	[IRQ_TTC1_2]		= GIC_LEVEL_SENSITIVE,
	[IRQ_DMAC4]		= GIC_LEVEL_SENSITIVE,
	[IRQ_DMAC5]		= GIC_LEVEL_SENSITIVE,
	[IRQ_DMAC6]		= GIC_LEVEL_SENSITIVE,
	[IRQ_DMAC7]		= GIC_LEVEL_SENSITIVE,
	[IRQ_USB1]		= GIC_LEVEL_SENSITIVE,
	[IRQ_ETHERNET1]		= GIC_LEVEL_SENSITIVE,
	[IRQ_ETHERNET1_WAKEUP]	= GIC_EDGE_SENSITIVE,
	[IRQ_SDIO1]		= GIC_LEVEL_SENSITIVE,
	[IRQ_I2C1]		= GIC_LEVEL_SENSITIVE,
	[IRQ_SPI1]		= GIC_LEVEL_SENSITIVE,
	[IRQ_UART1]		= GIC_LEVEL_SENSITIVE,
	[IRQ_CAN1]		= GIC_LEVEL_SENSITIVE,
	[IRQ_PL8]		= GIC_EDGE_SENSITIVE,
	[IRQ_PL9]		= GIC_EDGE_SENSITIVE,
	[IRQ_PL10]		= GIC_EDGE_SENSITIVE,
	[IRQ_PL11]		= GIC_EDGE_SENSITIVE,
	[IRQ_PL12]		= GIC_EDGE_SENSITIVE,
	[IRQ_PL13]		= GIC_EDGE_SENSITIVE,
	[IRQ_PL14]		= GIC_EDGE_SENSITIVE,
	[IRQ_PL15]		= GIC_EDGE_SENSITIVE,
	[IRQ_PARITY]		= GIC_EDGE_SENSITIVE
};


struct GicConfiguration
{
	int maxInterrupts;
} config;

static void EnableGic(void);
static void DisableGic(void);
static inline void EnableGicDistributor(void);
static inline void DisableGicDistributor(void);
static void ReadConfiguration(void);
static int ReadInterruptSource(void);
static void SetInterruptPriority(int irq, int prio);
static void SetInterruptCpuTarget(int irq, int cpu);
static void SetInterruptTrigger(int irq, int config);
static void EnableInterruptSource(int irq);
static void DisableInterruptSource(int irq);
static int IsInterruptSourceEnabled(int irq);
static inline void SignalEndOfInterrupt(int irq);

/*===========================================================================*
 *				intr_init				     *
 *===========================================================================*/
PUBLIC void intr_init(int mine)
{
	(void)mine;

	intr_disable();
	ReadConfiguration();
	DisableGic();
	/* Set disable to all the interrupts */
	volatile uint32_t *icdicer = (volatile uint32_t *) ICDICER0;
	for (int i = 0; i < config.maxInterrupts/32; i++)
	{
		*icdicer = ~0U;
	}
	/* Set priority mask */
	*(volatile uint32_t *)ICCPMR = GIC_DEFAULT_INTERRUPT_MASK;
	EnableGic();
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
/* Call the interrupt handlers for an interrupt with the given hook list.
 * The assembly part of the handler has already masked the IRQ, reenabled the
 * controller(s) and enabled interrupts.
 */
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

/*==========================================================================*
 *				enable_irq				    *
 *==========================================================================*/
PUBLIC void enable_irq(irq_hook_t *hook)
/* Enable an interrupt request line.
 */
{
	int irq = hook->irq;
	if ((irq_actids[irq] &= ~hook->id) == 0)
	{
		DisableGicDistributor();
		DisableInterruptSource(irq);
		SetInterruptPriority(irq, GIC_DEFAULT_INTERRUPT_PRIORITY);
		SetInterruptTrigger(irq, sensitivities[irq]);
		EnableInterruptSource(irq);
		EnableGicDistributor();
	}
}

/*==========================================================================*
 *				disable_irq				    *
 *==========================================================================*/
PUBLIC int disable_irq(irq_hook_t *hook)
/* Disable an interrupt request line.
 * Returns true iff the interrupt was not already disabled.
 */
{
	int ret;
	int irq = hook->irq;
	irq_actids[hook->irq] |= hook->id;
	DisableGicDistributor();
	ret = IsInterruptSourceEnabled(irq);
	DisableInterruptSource(irq);
	EnableGicDistributor();
	return ret;
}

/*===========================================================================*
 *			generic_interrupt_handler		     	     *
 *===========================================================================*/
PUBLIC void generic_interrupt_handler(void)
{
	irq_hook_t *hook;
	int irq;

	irq = ReadInterruptSource();
	kprintf("generic_interrupt_handler: irq %d has arrived", irq);
	hook = irq_handlers[irq];
	intr_handle(hook);
	if (irq_actids[irq] != 0)
	{
		kprintf("\nirq_actids[irq] != 0\n");
		DisableGicDistributor();
		DisableInterruptSource(irq);
		EnableGicDistributor();
	}
	SignalEndOfInterrupt(irq);
}


/*===========================================================================*
 *			System-dependant functions			     *
 *===========================================================================*/

static void EnableGic(void)
{
	EnableGicDistributor();
	*((volatile uint32_t *) ICCICR) = 1;
}

static void DisableGic(void)
{
	DisableGicDistributor();
	*((volatile uint32_t *) ICCICR) = 0;
}

static inline void EnableGicDistributor(void)
{
	*((volatile uint32_t *) ICDDCR) = 1;
}

static inline void DisableGicDistributor(void)
{
	*((volatile uint32_t *) ICDDCR) = 0;
}

static void ReadConfiguration(void)
{
	uint32_t icdictr = *((volatile uint32_t *) ICDICTR);
	config.maxInterrupts = ((icdictr & 0x1F) + 1) * 32;
}

static int ReadInterruptSource(void)
{
	return *((volatile uint32_t *) ICCIAR) & 0x3FF;
}

static void SetInterruptPriority(int irq, int prio)
{
	volatile uint32_t *icdipr = (volatile uint32_t *) ICDIPR0;
	int inx = irq / 4;
	int off = (irq % 4) * 8;
	uint32_t r = icdipr[inx];
	r &= ~(0xFFU << off);
	r |= (prio << off);
	icdipr[inx] = r;
}

static void SetInterruptCpuTarget(int irq, int cpu)
{
	volatile uint32_t *icdiptr = (volatile uint32_t *) ICDIPTR0;
	int inx = irq / 4;
	int off = (irq % 4) * 8;
	uint32_t r = icdiptr[inx];
	r &= ~(0xFFU << off);
	r |= (cpu << off);
	icdiptr[inx] = r;
}

static void SetInterruptTrigger(int irq, int config)
{
	volatile uint32_t *icdicfr = (volatile uint32_t *) ICDICFR0;
	int inx = irq / 16;
	int off = (irq % 16) * 2;
	uint32_t r = icdicfr[inx];
	r &= ~(0x3U << off);
	r |= (config << off);
	icdicfr[inx] = r;
}

static void EnableInterruptSource(int irq)
{
	volatile uint32_t *icdiser = (volatile uint32_t *) ICDISER0;
	icdiser[irq/32] |= (1 << (irq % 32));
}

static void DisableInterruptSource(int irq)
{
	volatile uint32_t *icdicer = (volatile uint32_t *) ICDICER0;
	icdicer[irq/32] |= (1 << (irq % 32));
}

static int IsInterruptSourceEnabled(int irq)
{
	volatile const uint32_t *icdiser = (volatile uint32_t *) ICDISER0;
	return icdiser[irq/32] & (1 << (irq % 32));
}

static inline void SignalEndOfInterrupt(int irq)
{
	*(volatile uint32_t *)ICCEOIR = irq;
}
