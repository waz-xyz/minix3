	.arch armv7a
	.syntax unified

	.section .text

	.global sigreturn
	.type	sigreturn, %function

	.align	2

sigreturn:
	ldr	pc, =_sigreturn
