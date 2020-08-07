	.arch armv7a
	.syntax unified

	.section .text

	.global execle
	.type	execle, %function

	.align	2

execle:
	ldr	pc, =_execle
