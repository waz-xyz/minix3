	.arch armv7a
	.syntax unified

	.section .text

	.global sigdelset
	.type	sigdelset, %function

	.align	2

sigdelset:
	ldr	pc, =_sigdelset
