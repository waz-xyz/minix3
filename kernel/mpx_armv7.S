#include <minix/config.h>
#include <minix/const.h>
#include <minix/com.h>
#include "const.h"

	.arch armv7a
	.syntax unified

	.section .text.boot

	.global _start
	.type	_start, %function

/*===========================================================================*
 *				SECTION .text.boot			     *
 *===========================================================================*/
_start:			/* this is the entry point for the MINIX kernel */
	@ blx	serial_init
	@ ldr	r0, =welcome
	@ blx	serial_puts

	@ ldr	r0, =sID_MMFR0
	@ blx	serial_puts
	@ mrc	p15, 0, r0, c0, c1, 4
	@ blx	serial_printHex

	@ ldr	r0, =sTTBCR
	@ blx	serial_puts
	@ mrc	p15, 0, r0, c2, c0, 2
	@ blx	serial_printHex

	@ ldr	r0, =sTTBR0
	@ blx	serial_puts
	@ mrc	p15, 0, r0, c2, c0, 0
	@ blx	serial_printHex

	@ ldr	r0, =sTTBR1
	@ blx	serial_puts
	@ mrc	p15, 0, r0, c2, c0, 1
	@ blx	serial_printHex

	@ ldr	r0, =sSCTLR
	@ blx	serial_puts
	@ mrc	p15, 0, r0, c1, c0, 0
	@ blx	serial_printHex

	@ ldr	r0, =sDACR
	@ blx	serial_puts
	@ mrc	p15, 0, r0, c3, c0, 0
	@ blx	serial_printHex

	/* Turn on the red LED */
	mov	r0, #1
	ldr	r1, =#0x41210000
	str	r0, [r1]

	/* Enable TTBR1 in TTBCR */
	mrc	p15, 0, r0, c2, c0, 2
	orr	r0, r0, #1
	mcr	p15, 0, r0, c2, c0, 2

	/* Set TTBR1 base address at 0x4000 */
	mov	r0, #0x4000
	mcr	p15, 0, r0, c2, c0, 1

	/* Initialize the page tables to zero */
	mov	r1, #0x8400
	bl	memclr

	/* Set page table descriptor for L1 table index = 0x800
	   with base address at 0x8000 */
	mov	r1, #0x6000
	ldr	r0, =#0x8001
	str	r0, [r1]

	/* Set large page table entries for L2 table index = 0x00
	   with base address at 0x100000. Large page for kernel code. */
	mov	r2, #16
	mov	r1, #0x8000
	ldr	r0, =#0x105615
1:	str	r0, [r1], #4
	subs	r2, #1
	bne	1b

	/* Set large page table entries for L2 table index = 0x10
	   with base address at 0x110000. Large page for kernel data. */
	mov	r2, #16
	ldr	r0, =#0x115415
1:	str	r0, [r1], #4
	subs	r2, #1
	bne	1b

	/* Set small page table entry for L2 table index = 0x21
	   with base address at 0xFFF000. Small page for kernel stack. */
	ldr	r1, =#0x8084
	ldr	r0, =#0xFFF556
	str	r0, [r1]

	/* Set small page table entry for L2 table index = 0x22
	   with base address at 0x41210000. Small page for LED output. */
	ldr	r1, =#0x8088
	ldr	r0, =#0x41210016
	str	r0, [r1]

	/* Set small page table entry for L2 table index = 0x23
	   with base address at 0xE0000000. Small page for PS UART. */
	ldr	r1, =#0x808C
	ldr	r0, =#0xE0000016
	str	r0, [r1]

	/* Set domains through DACR */
	mov	r0, #0xFFFFFFFF
	mcr	p15, 0, r0, c3, c0, 0

	/* Set kernel stack */
	ldr	sp, =#0x80022000

	/* Invalidate TLB (TLBIALL) */
	mcr	p15, 0, r0, c8, c7, 0

	/* Enable MMU and caches through SCTLR */
	mrc	p15, 0, r0, c1, c0, 0
	mov	r1, #0x1005
	orr	r0, r0, r1
	mcr	p15, 0, r0, c1, c0, 0

	ldr	pc, =mmu_is_on
mmu_is_on:
	/* Barriers */
	dsb
	isb

	/* Invalidate instruction cache (ICIALLU) */
	mcr	p15, 0, r0, c7, c5, 0

	/* Turn on the green LED */
	mov	r0, #2
	bl	SetLeds

	/* Clear BSS memory */
	ldr	r0, =__bss_start
	ldr	r1, =__bss_end
	bl	memclr

	@ blx	serial_init
	@ ldr	r0, =welcome
	@ blx	serial_puts

	/* Jump to C entry point */
	ldr	r0, =cstart
	blx	r0

forever:
	b	forever

SetLeds:
	ldr	r1, =#0x80022000
	str	r0, [r1]
	bx	lr

memclr:
	mov	r5, #0
	mov	r6, #0
	mov	r7, #0
	mov	r8, #0
1:	stmia	r0!, {r5-r8}
	cmp	r0, r1
	blo	1b
	bx	lr

welcome:	.asciz	"Starting kernel..."
sID_MMFR0:	.asciz	"ID_MMFR0:"
sTTBCR:		.asciz	"TTBCR:"
sTTBR0:		.asciz	"TTBR0:"
sTTBR1:		.asciz	"TTBR1:"
sSCTLR:		.asciz	"SCTLR:"
sDACR:		.asciz	"DACR:"

/*===========================================================================*
 *				SECTION .text				     *
 *===========================================================================*/

	.section .text

	.global restart
	.type	restart, %function


/*===========================================================================*
 *				restart					     *
 *===========================================================================*
 * Restart the current process or the next process if it is set. 
 */
restart:
        bx      lr
