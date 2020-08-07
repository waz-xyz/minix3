	.arch armv7a
	.syntax unified

	.section .text

	.global unlink
	.type	unlink, %function

	.align	2

unlink:
	ldr	pc, =_unlink
