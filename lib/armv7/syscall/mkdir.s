	.arch armv7a
	.syntax unified

	.section .text

	.global mkdir
	.type	mkdir, %function

	.align	2

mkdir:
	ldr	pc, =_mkdir
