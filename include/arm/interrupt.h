/* Interrupt numbers and hardware vectors. */

#ifndef _INTERRUPT_H
#define _INTERRUPT_H

#if (CHIP == ARM)

#if (MACHINE == ZYNQ)

#define	GIC_DEFAULT_INTERRUPT_PRIORITY	(15 * 8)
#define	GIC_DEFAULT_INTERRUPT_MASK	(16 * 8)

#define	GIC_SGI_EDGE_SENSITIVE		2
#define	GIC_LEVEL_SENSITIVE		1
#define	GIC_EDGE_SENSITIVE		3

/*******************************
 * Hardware interrupt numbers. *
 *******************************/

/* Software-generated interrupts */
#define IRQ_SGI0		0
#define IRQ_SGI1		1
#define IRQ_SGI2		2
#define IRQ_SGI3		3
#define IRQ_SGI4		4
#define IRQ_SGI5		5
#define IRQ_SGI6		6
#define IRQ_SGI7		7
#define IRQ_SGI8		8
#define IRQ_SGI9		9
#define IRQ_SGI10		10
#define IRQ_SGI11		11
#define IRQ_SGI12		12
#define IRQ_SGI13		13
#define IRQ_SGI14		14
#define IRQ_SGI15		15

/* Private Peripheral Interrupts (PPIs) */
#define IRQ_GLOBAL_TIMER	27
#define	IRQ_NFIQ		28
#define	IRQ_PRIVATE_TIMER	29
#define	IRQ_AWDT		30
#define	IRQ_NIRQ		31

/* Shared Peripheral Interrupts (SPIs) */
#define	IRQ_CPU0		32
#define	IRQ_CPU1		33
#define	IRQ_L2CACHE		34
#define	IRQ_OCM			35
#define	IRQ_PMU0		37
#define	IRQ_PMU1		38
#define	IRQ_XADC		39
#define	IRQ_DEVC		40
#define	IRQ_SWDT		41
#define	IRQ_TTC0_0		42
#define	IRQ_TTC0_1		43
#define	IRQ_TTC0_2		44
#define	IRQ_DMAC_ABORT		45
#define	IRQ_DMAC0		46
#define	IRQ_DMAC1		47
#define	IRQ_DMAC2		48
#define	IRQ_DMAC3		49
#define	IRQ_SMC			50
#define	IRQ_QSPI		51
#define	IRQ_GPIO		52
#define	IRQ_USB0		53
#define	IRQ_ETHERNET0		54
#define	IRQ_ETHERNET0_WAKEUP	55
#define	IRQ_SDIO0		56
#define	IRQ_I2C0		57
#define	IRQ_SPI0		58
#define	IRQ_UART0		59
#define	IRQ_CAN0		60
#define	IRQ_PL0			61
#define	IRQ_PL1			62
#define	IRQ_PL2			63
#define	IRQ_PL3			64
#define	IRQ_PL4			65
#define	IRQ_PL5			66
#define	IRQ_PL6			67
#define	IRQ_PL7			68
#define	IRQ_TTC1_0		69
#define	IRQ_TTC1_1		70
#define	IRQ_TTC1_2		71
#define	IRQ_DMAC4		72
#define	IRQ_DMAC5		73
#define	IRQ_DMAC6		74
#define	IRQ_DMAC7		75
#define	IRQ_USB1		76
#define	IRQ_ETHERNET1		77
#define	IRQ_ETHERNET1_WAKEUP	78
#define	IRQ_SDIO1		79
#define	IRQ_I2C1		80
#define	IRQ_SPI1		81
#define	IRQ_UART1		82
#define	IRQ_CAN1		83
#define	IRQ_PL8			84
#define	IRQ_PL9			85
#define	IRQ_PL10		86
#define	IRQ_PL11		87
#define	IRQ_PL12		88
#define	IRQ_PL13		89
#define	IRQ_PL14		90
#define	IRQ_PL15		91
#define	IRQ_PARITY		92

#define NR_IRQ_VECTORS		93

#endif  /* (MACHINE == ZYNQ) */

#endif /* (CHIP == ARM) */

#endif /* _INTERRUPT_H */
