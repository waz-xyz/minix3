/* Interrupt numbers and hardware vectors. */

#ifndef _INTERRUPT_H
#define _INTERRUPT_H

#if (CHIP == ARM)

/* Hardware interrupt numbers. */
#define NR_IRQ_VECTORS    16
#define CLOCK_IRQ          0
#define KEYBOARD_IRQ       1
#define CASCADE_IRQ        2	/* cascade enable for 2nd AT controller */
#define ETHER_IRQ          3	/* default ethernet interrupt vector */
#define SECONDARY_IRQ      3	/* RS232 interrupt vector for port 2 */
#define RS232_IRQ          4	/* RS232 interrupt vector for port 1 */
#define XT_WINI_IRQ        5	/* xt winchester */
#define FLOPPY_IRQ         6	/* floppy disk */
#define PRINTER_IRQ        7
#define KBD_AUX_IRQ       12	/* AUX (PS/2 mouse) port in kbd controller */
#define AT_WINI_0_IRQ     14	/* at winchester controller 0 */
#define AT_WINI_1_IRQ     15	/* at winchester controller 1 */

#define RESET_EXCEPTION			0
#define UNDEFINED_INSTRUCTION_EXCEPTION	1
#define	SUPERVISOR_CALL_EXCEPTION	2
#define	PREFETCH_ABORT_EXCEPTION	3
#define	DATA_ABORT_EXCEPTION		4
#define	IRQ_EXCEPTION			5
#define	FAST_IRQ_EXCEPTION		6

#endif /* (CHIP == ARM) */

#endif /* _INTERRUPT_H */
