#include <minix/config.h>
#include <minix/const.h>
#include <minix/com.h>
#include "const.h"

	.arch armv7a
	.syntax unified

	.section .text.boot

	.global _start
	.type	_start, %function

	.global	end_of_image
	.global	number_of_programs
	.global	programs_locations

/*===========================================================================*
 *				SECTION .text.boot			     *
 *===========================================================================*/
	/* This is the entry point for the MINIX kernel */
_start:
	b	skip_header

	/* To be filled by mkimage: offset to end of image */
end_of_image:
	.word	0

	/* To be filled by mkimage: number of executables loaded */
number_of_programs:
	.word	0

	/* To be filled by mkimage: offset of headers' locations */
programs_locations:
	.word	0	/* kernel */
	.word	0	/* pm */
	.word	0	/* fs */
	.word	0	/* rs */
	.word	0	/* ds */
	.word	0	/* tty */
	.word	0	/* mem */
	.word	0	/* log */
	.word	0	/* init */

skip_header:
	/* Turn on the red LED */
	mov	r0, #1
	ldr	r1, =#0x41210000
	str	r0, [r1]

	/* Enable TTBR1 in TTBCR */
	mrc	p15, 0, r0, c2, c0, 2
	orr	r0, r0, #1
	mcr	p15, 0, r0, c2, c0, 2

	/* Get end of image in physical memory */
	ldr	r0, =end_of_image
	ldr	r1, =#KERNEL_PHYSICAL_OFFSET
	sub	r0, r1
	ldr	r0, [r0]
	add	r0, #KERNEL_PHYSICAL_BASE

	/* Find the next free location for the kernel stack pages
	   by aligning to 4k. Preserve the value at r9. */
	mov	r1, #SMALL_PAGE_ALIGN
	add	r0, r1
	ldr	r1, =#~(FIRST_LEVEL_TABLE_ALIGN)
	and	r0, r1
	mov	r9, r0
	add	r0, KERNEL_STACK_SIZE

	/* Find the next free location for a first-level table
	   by aligning to 16k. Preserve the value at r8. */
	mov	r1, #FIRST_LEVEL_TABLE_ALIGN
	add	r0, r1
	ldr	r1, =#~FIRST_LEVEL_TABLE_ALIGN
	and	r0, r1
	mov	r8, r0

	/* Set TTBR1 base address at the last found location */
	mcr	p15, 0, r0, c2, c0, 1

	/* Initialize the page tables to zero */
	add	r1, r0, #KERNEL_PAGE_TABLES_SIZE
	bl	memclr
	
	/* Set page table descriptor for L1 table index = 0x800
	   with base address at the end of the first-level table.
	   Preserve the page table location at r7. */
	add	r1, r8, #(4 * 0x800)
	add	r7, r8, #FIRST_LEVEL_TABLE_SIZE
	add	r0, r7, #1		/* Make it point to a page table */
	str	r0, [r1]

	/* Set section descriptors for L1 table index = 0x810 and beyond
	   with base addresses starting at KERNEL_PHYSICAL_BASE.
	   Enable access to the first 16 MBs of physical memory. */
	mov	r2, #16
	add	r1, r8, #(4 * 0x810)
	mov	r3, #KERNEL_PHYSICAL_BASE
	ldr	r4, =#0x540A
1:	add	r0, r3, r4
	str	r0, [r1], #4
	add	r3, #ARM_SECTION_SIZE
	subs	r2, #1
	bne	1b

	/* Set large page table entries for L2 table index = 0x00
	   with base address at 0x100000. Large page for kernel code. */
	mov	r2, #16
	mov	r1, r7
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

	/* Set small page table entries for L2 table index = 0x21 and beyond
	   with base addresses starting at r9. Small pages for kernel stack. */
	ldr	r4, =#0x556
	mov	r3, r9
	mov	r2, #KERNEL_NUM_STACK_PAGES
	add	r1, r7, #(4 * 0x21)
one_more_page:
	add	r0, r3, r4
	str	r0, [r1], #4
	add	r3, #SMALL_PAGE_SIZE
	subs	r2, #1
	bne	one_more_page

	/* Set small page table entry for L2 table index = 0x22
	   with base address at 0x41210000. Small page for LED output. */
	add	r1, r7, #(4 * 0x22)
	ldr	r0, =#0x41210016
	str	r0, [r1]

	/* Set small page table entry for L2 table index = 0x23
	   with base address at 0xE0000000. Small page for PS UART. */
	add	r1, r7, #(4 * 0x23)
	ldr	r0, =#0xE0000016
	str	r0, [r1]

	/* Set domains through DACR */
	mov	r0, #0xFFFFFFFF
	mcr	p15, 0, r0, c3, c0, 0

	/* Set kernel stack */
	ldr	sp, =#INITIAL_KERNEL_SP

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

	mov	r0, r9	/* physical location of kernel stack */
	mov	r1, r8	/* physical location of kernel's first-level MMU table  */
	mov	r2, r7	/* physical location of kernel's first page table */
	/* Jump to C entry point */
	ldr	r5, =cstart
	blx	r5

forever:
	b	forever

SetLeds:
	ldr	r1, =#0x80022000
	str	r0, [r1]
	bx	lr

memclr:
	mov	r2, #0
	mov	r3, #0
	mov	r4, #0
	mov	r5, #0
1:	stmia	r0!, {r2-r5}
	cmp	r0, r1
	blo	1b
	bx	lr

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
