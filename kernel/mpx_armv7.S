#include <minix/config.h>
#include <minix/const.h>
#include <minix/com.h>
#include "const.h"
#include "sconst.h"

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
	ldr	r1, =#~SMALL_PAGE_ALIGN
	and	r0, r1
	mov	r9, r0
	add	r0, KERNEL_STACK_SIZE

	/* Find the next free location for a first-level table
	   by aligning to 16k. Preserve the value at r8. */
	mov	r1, #KERNEL_FIRST_LEVEL_TT_ALIGN
	add	r0, r1
	ldr	r1, =#~KERNEL_FIRST_LEVEL_TT_ALIGN
	and	r0, r1
	mov	r8, r0

	/* Set TTBR1 base address at the last found location */
	mcr	p15, 0, r0, c2, c0, 1

	/* Initialize the page tables to zero */
	add	r1, r0, #KERNEL_MMU_TABLES_SIZE
	bl	memclr
	
	/* Set page table descriptor for L1 table index = 0x800
	   with base address at the end of the first-level table.
	   Preserve the page table location at r7. */
	add	r1, r8, #(4 * 0x800)
	add	r7, r8, #KERNEL_FIRST_LEVEL_TT_SIZE
	add	r0, r7, #1		/* Make it point to a page table */
	str	r0, [r1]

	/* Set page table descriptor for L1 table index = 0xFFF
	   with base address at the end of the first page table. */
	mov	r1, #(4 * 0xFFF)
	add	r1, r8
	add	r0, r7, #PAGE_TABLE_SIZE
	add	r0, r0, #1		/* Make it point to a page table */
	str	r0, [r1]

	/* Set section descriptors for L1 table index = 0x810 and beyond
	   with base addresses starting at 0.
	   Enable access to the first 16 MBs of physical memory. */
	mov	r2, #16
	add	r1, r8, #(4 * 0x810)
	mov	r3, #0
	ldr	r4, =#0x640A
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

	/* Set V = 1 in SCTLR (high exception vectors) */
	mrc	p15, 0, r0, c1, c0, 0
	orr	r0, #(1 << 13)
	mcr	p15, 0, r0, c1, c0, 0

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
 *				save					     *
 *===========================================================================*
 * Save registers upon entering the kernel.
 */
save:
	push	{lr}
	ldr	lr, =proc_ptr
	ldr	lr, [lr]		// lr = proc_ptr
	cmp	lr, 0
	popeq	{lr}
	beq	0f			// if proc_ptr == NULL, we have a problem
	stm	lr, {r0-r14}^		// proc_ptr->p_reg.r[0..14] = r[0..14]_usr
	add	lr, #(15 * 4)
	add	sp, #4			// skip pushed lr
	pop	{r0-r1}			// Store saved pc(return),spsr
	stm	lr, {r0-r1}		//  at proc_ptr->p_reg.[pc,psw]
	ldr	lr, [sp, #-12]		// load pushed lr
0:
	bx	lr

/*===========================================================================*
 *				restart					     *
 *===========================================================================*
 * Restart the current process or the next process if it is set. 
 */
restart:
	ldr	r0, =proc_ptr		// r0 = &proc_ptr
	ldr	r1, =next_ptr		// r1 = &next_ptr
	ldr	r2, [r1]		// r2 = next_ptr
	cmp	r2, 0			// see if another process is scheduled
	beq	0f
	str	r2, [r0]		// schedule new process: proc_ptr = next_ptr
	mov	r2, 0
	str	r2, [r1]		// next_ptr = 0
0:	ldr	r0, [r0]		// r0 = proc_ptr
	// at this point r0 = &(proc_ptr->p_reg.r0)
restart1:
	// k_reenter -= 1
	ldr	r1, =k_reenter
	ldr	r2, [r1]
	sub	r2, 1
	str	r2, [r1]

	// If proc_ptr->p_nr < 0, we are switching to a kernel task,
	// so we can skip the MMU context switch. Keep p_nr at r1.
	ldr	r1, [r0, #P_NR_OFFSET]
	cmp	r1, 0
	blt	1f

	// get current ASID from CONTEXTDIR and keep at r4
	mrc	p15, 0, r4, c13, c0, 1
	and	r4, #0xFF

	// set ASID in CONTEXTDIR
	and	r1, #0xFF		// use the lower 8 bits of proc_ptr->p_nr
	cmp	r1, r4			// compare current ASID with ASID of next process
	beq	1f			// if they are the same, skip the MMU context switch
	mcr	p15, 0, r1, c13, c0, 1

	// set TTBR0 base address to proc_ptr->p_ttbase
	ldr	r2, [r0, #P_TTBASE_OFFSET]
	mcr	p15, 0, r2, c2, c0, 0

	// invalidate TLB by ASID
	mcr	p15, 0, r4, c8, c7, 2	// execute TLBIASID
	isb				// make effective every instruction up to this point

	// return sp of kernel to its empty location
	ldr	sp, =INITIAL_KERNEL_SP

1:	// (re)load registers for the process
	mov	lr, r0			// use lr_svc because is different from lr_usr
	ldm	lr, {r0-r14}^		// r[0..14]_usr = proc_ptr->p_reg.r[0..14]
	add	lr, #PCREG_OFFSET	// r0 = &(proc_ptr->p_reg.pc)
	rfe	lr			// continue process:
					//	pc_usr = proc_ptr->p_reg.pc
					//	cpsr_usr = proc_ptr->p_reg.psw
	@ mov	r1, r0
	@ ldr	r2, [r0, #P_NR_OFFSET]
	@ ldr	r3, [r0, #PCREG_OFFSET]
	@ ldr	r0, =mystr
	@ blx	kprintf
	@ mov	r0, 1
	@ bl	SetLeds
	@ b	.
@ mystr:	.asciz "proc_ptr = 0x%08X, nr = %d, next_pc = 0x%08X\n"
@ 	.align 4

/*===========================================================================*
 *				exception handlers		     	     *
 *===========================================================================*/

	.global	reset_handler
	.global	undefined_instruction_handler
	.global	supervisor_call_handler
	.global	prefetch_abort_handler
	.global	data_abort_handler
	.global	irq_handler
	.global	fast_irq_handler

	.type	reset_handler, %function
	.type	undefined_instruction_handler, %function
	.type	supervisor_call_handler, %function
	.type	prefetch_abort_handler, %function
	.type	data_abort_handler, %function
	.type	irq_handler, %function
	.type	fast_irq_handler, %function

reset_handler:
	srsdb	sp!, #MODE_SVC
	cps	#MODE_SVC
	bl	save
	mov	r0, RESET_EXCEPTION
	b	generic_exception_handler

undefined_instruction_handler:
	srsdb	sp!, #MODE_SVC
	cps	#MODE_SVC
	bl	save
	mov	r0, UNDEFINED_INSTRUCTION_EXCEPTION
	b	generic_exception_handler

supervisor_call_handler:
	srsdb	sp!, #MODE_SVC
	bl	save
	mov	r0, SUPERVISOR_CALL_EXCEPTION
	b	generic_exception_handler

prefetch_abort_handler:
	srsdb	sp!, #MODE_SVC
	cps	#MODE_SVC
	bl	save
	mov	r0, PREFETCH_ABORT_EXCEPTION
	b	generic_exception_handler

data_abort_handler:
	srsdb	sp!, #MODE_SVC
	cps	#MODE_SVC
	bl	save
	mov	r0, DATA_ABORT_EXCEPTION
	b	generic_exception_handler

irq_handler:
	sub	lr, #4
	srsdb	sp!, #MODE_SVC
	cps	#MODE_SVC
	bl	save
	mov	r0, IRQ_EXCEPTION
	b	generic_exception_handler

fast_irq_handler:
	sub	lr, #4
	srsdb	sp!, #MODE_SVC
	cps	#MODE_SVC
	bl	save
	mov	r0, FAST_IRQ_EXCEPTION
	// Fall into generic exception handling

generic_exception_handler:
	@ // Turn on the blue LED
	@ mov	r5, #4
	@ ldr	r6, =#0x80022000
	@ str	r5, [r6]
	// k_reenter += 1; keep the new value at r4 (callee-saved)
	ldr	r1, =k_reenter
	ldr	r4, [r1]
	add	r4, 1
	str	r4, [r1]
	// The stack must be aligned at 8 bytes by ARM ABI requirements.
	// Keep the offset at r5 (callee-saved).
	and	r5, sp, #4
	sub	sp, r5
	ldr	r1, =exception
	blx	r1
	add	sp, r5
	// If ++k_reenter != 0 we are reentering the kernel
	cmp	r4, 0
	beq	restart
	bne	restart1

// TODO: Make it useful
generic_interrupt_handler:
	push	{r0-r4, r12, lr}
	and	r4, sp, #4
	sub	sp, r4
	ldr	lr, =intr_handle
	blx	lr
	add	sp, r4
	pop	{r0-r4, r12, lr}
	rfe	sp!

	.global force_undefined
	.type	force_undefined, %function
force_undefined:
	mrc	p8, 0, r0, c0, c0, 0
	bx	lr

	.global force_data_abort
	.type	force_data_abort, %function
force_data_abort:
	mov	r0, #0
	ldr	r0, [r0]
	bx	lr
