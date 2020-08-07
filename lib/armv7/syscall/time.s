	.arch armv7a
	.syntax unified

	.section .text

	.global time
	.type	time, %function

	.align	2

time:
	ldr	pc, =_time
