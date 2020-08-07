#include "kernel.h"

#if (MACHINE == IBM_PC)

#define COM1_BASE       0x3F8
#define COM1_THR	(COM1_BASE + 0)
#define LSR_THRE	0x20
#define COM1_LSR	(COM1_BASE + 5)

void serial_putc(char c)
{
	int i;
	int lsr, thr;

	lsr= COM1_LSR;
	thr= COM1_THR;
	for (i= 0; i<100000; i++)
	{
		if (inb(lsr) & LSR_THRE)
			break;
	}
	outb(thr, c);
}

#elif (MACHINE == ZYNQ)

void serial_putc(char c)
{
        (void)c;
}

#endif