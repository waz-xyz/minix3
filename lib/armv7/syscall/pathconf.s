	.arch armv7a
	.syntax unified

	.section .text

	.global pathconf
	.type	pathconf, %function

	.align	2

pathconf:
	ldr	pc, =_pathconf
