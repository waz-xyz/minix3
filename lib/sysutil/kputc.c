/* A server must occasionally print some message.  It uses a simple version of 
 * printf() found in the system lib that calls kputc() to output characters.
 * Printing is done with a call to the kernel, and not by going through FS.
 *
 * This routine can only be used by servers and device drivers.  The kernel
 * must define its own kputc(). Note that the log driver also defines its own 
 * kputc() to directly call the TTY instead of going through this library.
 */

#include "sysutil.h"

enum
{
	// The base address for Zynq's UART0.
	UART0_BASE = 0xC0000000U + 0x100000U - 0x1000U - 3 * 0x1000U,
	UART0_SR = (UART0_BASE + 0x2C),
	UART0_FIFO = (UART0_BASE + 0x30)
};

static inline void io_write(unsigned reg, unsigned data)
{
	*(volatile unsigned *)reg = data;
}

static inline unsigned io_read(unsigned reg)
{
	return *(volatile unsigned *)reg;
}

static void serial_putc(int c)
{
	while ((io_read(UART0_SR) & (1 << 4)) != 0)
	{
		;
	}
	io_write(UART0_FIFO, c);
}

void kputc(int c)
{
	serial_putc(c);
	if (c == '\n') serial_putc('\r');
}

/*===========================================================================*
 *				kputc					     *
 *===========================================================================*/
// void kputc(int c)
// /* Accumulate another character. If 0 or buffer full, print it. */
// {
// 	static int buf_count;		/* # characters in the buffer */
// 	static char print_buf[80];	/* output is buffered here */
// 	message m;

// 	if ((c == 0 && buf_count > 0) || buf_count == sizeof(print_buf))
// 	{
// 		int procs[] = OUTPUT_PROCS_ARRAY;
// 		int p;

// 		for (p = 0; procs[p] != NONE; p++)
// 		{
// 			/* Send the buffer to this output driver. */
// 			m.DIAG_BUF_COUNT = buf_count;
// 			m.DIAG_PRINT_BUF = print_buf;
// 			m.DIAG_ENDPT = SELF;
// 			m.m_type = DIAGNOSTICS;
// 			(void)_sendrec(procs[p], &m);
// 		}
// 		buf_count = 0;

// 	/* If the output fails, e.g., due to an ELOCKED, do not retry output
//          * at the FS as if this were a normal user-land printf(). This may 
//          * result in even worse problems. 
//          */
// 	}
// 	if (c != 0)
// 	{
// 		/* Append a single character to the output buffer. */
// 		print_buf[buf_count++] = c;
// 	}
// }
