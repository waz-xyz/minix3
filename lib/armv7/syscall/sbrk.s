	.arch armv7a
	.syntax unified

	.section .text

	.global sbrk
	.type	sbrk, %function

	.align	2

sbrk:
	ldr	pc, =_sbrk
