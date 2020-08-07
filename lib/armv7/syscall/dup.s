	.arch armv7a
	.syntax unified

	.section .text

	.global dup
	.type	dup, %function

	.align	2

dup:
	ldr	pc, =_dup
