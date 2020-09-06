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

enum
{
	// The base address for Zynq's UART0.
	UART0_BASE	= KERNEL_VIRTUAL_BASE + ARM_SECTION_SIZE - KERNEL_STACK_SIZE - 3*SMALL_PAGE_SIZE,

	UART0_CR	= (UART0_BASE + 0x00),
	UART0_MR	= (UART0_BASE + 0x04),
	UART0_IER	= (UART0_BASE + 0x08),
	UART0_IDR	= (UART0_BASE + 0x0C),
	UART0_IMR	= (UART0_BASE + 0x10),
	UART0_ISR	= (UART0_BASE + 0x14),
	UART0_BAUDGEN	= (UART0_BASE + 0x18),
	UART0_RXTOUT	= (UART0_BASE + 0x1C),
	UART0_RXWM	= (UART0_BASE + 0x20),
	UART0_MODEMCR	= (UART0_BASE + 0x24),
	UART0_MODEMSR	= (UART0_BASE + 0x28),
	UART0_SR	= (UART0_BASE + 0x2C),
	UART0_FIFO	= (UART0_BASE + 0x30),
	UART0_BAUDDIV	= (UART0_BASE + 0x34),
	UART0_FLOWDEL	= (UART0_BASE + 0x38),
	UART0_TXWM	= (UART0_BASE + 0x44),
	UART0_RXBS	= (UART0_BASE + 0x48)
};

static inline void io_write(uint32_t reg, uint32_t data)
{
	*(volatile uint32_t*)reg = data;
}

static inline uint32_t io_read(uint32_t reg)
{
	return *(volatile uint32_t*)reg;
}

void serial_init(void)
{
	io_write(UART0_IDR, 0x1FFF);
}

void serial_putc(char c)
{
        while ((io_read(UART0_SR) & (1 << 4)) != 0)
	{
		;
	}
	io_write(UART0_FIFO, c);
}

void serial_puts(const char* str)
{
	for (size_t i = 0; str[i] != '\0'; i ++)
		serial_putc(str[i]);
	serial_putc('\r');
	serial_putc('\n');
}

void serial_printHex(uint32_t h)
{
	char s[9];

	s[8] = '\0';
	for (int i = 7; i >= 0; i--) {
		uint8_t nibble = h & 0xFU;
		s[i] = nibble > 9 ? (nibble - 10 + 'A') : (nibble + '0');
		h >>= 4;
	}
	serial_puts(s);
}

#endif