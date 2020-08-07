	.arch armv7a
	.syntax unified

	.section .text

	.global tcsetattr
	.type	tcsetattr, %function

	.align	2

tcsetattr:
	ldr	pc, =_tcsetattr
