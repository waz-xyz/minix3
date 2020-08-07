	.arch armv7a
	.syntax unified

	.section .text

	.global fpathconf
	.type	fpathconf, %function

	.align	2

fpathconf:
	ldr	pc, =_fpathconf
