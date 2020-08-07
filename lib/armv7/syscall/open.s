	.arch armv7a
	.syntax unified

	.section .text

	.global open
	.type	open, %function

	.align	2

open:
	ldr	pc, =_open
