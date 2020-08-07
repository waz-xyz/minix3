	.arch armv7a
	.syntax unified

	.section .text

	.global getsigset
	.type	getsigset, %function

	.align	2

getsigset:
	ldr	pc, =_getsigset
