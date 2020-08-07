	.arch armv7a
	.syntax unified

	.section .text

	.global sigaction
	.type	sigaction, %function

	.align	2

sigaction:
	ldr	pc, =_sigaction
