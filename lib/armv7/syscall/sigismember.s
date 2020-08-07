	.arch armv7a
	.syntax unified

	.section .text

	.global sigismember
	.type	sigismember, %function

	.align	2

sigismember:
	ldr	pc, =_sigismember
