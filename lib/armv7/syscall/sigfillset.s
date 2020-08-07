	.arch armv7a
	.syntax unified

	.section .text

	.global sigfillset
	.type	sigfillset, %function

	.align	2

sigfillset:
	ldr	pc, =_sigfillset
