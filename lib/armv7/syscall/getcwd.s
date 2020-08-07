	.arch armv7a
	.syntax unified

	.section .text

	.global getcwd
	.type	getcwd, %function

	.align	2

getcwd:
	ldr	pc, =_getcwd
