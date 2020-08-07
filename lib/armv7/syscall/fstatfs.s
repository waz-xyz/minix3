	.arch armv7a
	.syntax unified

	.section .text

	.global fstatfs
	.type	fstatfs, %function

	.align	2

fstatfs:
	ldr	pc, =_fstatfs
