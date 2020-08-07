	.arch armv7a
	.syntax unified

	.section .text

	.global access
	.type	access, %function

	.align	2

access:
	ldr	pc, =_access
