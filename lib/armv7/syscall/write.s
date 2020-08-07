	.arch armv7a
	.syntax unified

	.section .text

	.global write
	.type	write, %function

	.align	2

write:
	ldr	pc, =_write
