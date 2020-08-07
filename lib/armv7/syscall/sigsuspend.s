	.arch armv7a
	.syntax unified

	.section .text

	.global sigsuspend
	.type	sigsuspend, %function

	.align	2

sigsuspend:
	ldr	pc, =_sigsuspend
