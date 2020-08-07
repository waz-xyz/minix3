	.arch armv7a
	.syntax unified

	.section .text

	.global truncate
	.type	truncate, %function

	.global ftruncate
	.type	ftruncate, %function

	.align	2

truncate:
	ldr	pc, =_truncate

ftruncate:
	ldr	pc, =_ftruncate
