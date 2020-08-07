	.arch armv7a
	.syntax unified

	.section .text

	.global brk
	.type	brk, %function

	.align	2

brk:
	ldr	pc, =_brk
