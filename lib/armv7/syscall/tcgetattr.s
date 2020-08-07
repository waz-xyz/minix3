	.arch armv7a
	.syntax unified

	.section .text

	.global tcgetattr
	.type	tcgetattr, %function

	.align	2

tcgetattr:
	ldr	pc, =_tcgetattr
