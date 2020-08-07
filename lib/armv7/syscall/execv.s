	.arch armv7a
	.syntax unified

	.section .text

	.global execv
	.type	execv, %function

	.align	2

execv:
	ldr	pc, =_execv
