	.arch armv7a
	.syntax unified

	.section .text

	.global times
	.type	times, %function

	.align	2

times:
	ldr	pc, =_times
