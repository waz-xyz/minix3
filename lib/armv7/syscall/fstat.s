	.arch armv7a
	.syntax unified

	.section .text

	.global fstat
	.type	fstat, %function

	.align	2

fstat:
	ldr	pc, =_fstat
