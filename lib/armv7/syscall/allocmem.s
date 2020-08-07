	.arch armv7a
	.syntax unified

	.section .text

	.global allocmem
	.type	allocmem, %function

	.align	2

allocmem:
	ldr	pc, =_allocmem
