	.arch armv7a
	.syntax unified

	.section .text

	.global sigpending
	.type	sigpending, %function

	.align	2

sigpending:
	ldr	pc, =_sigpending
