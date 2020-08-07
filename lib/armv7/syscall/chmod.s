	.arch armv7a
	.syntax unified

	.section .text

	.global chmod
	.type	chmod, %function

	.align	2

chmod:
	ldr	pc, =_chmod
