	.arch armv7a
	.syntax unified

	.section .text

	.global stime
	.type	stime, %function

	.align	2

stime:
	ldr	pc, =_stime
