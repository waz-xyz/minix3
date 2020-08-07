	.arch armv7a
	.syntax unified

	.section .text

	.global freemem
	.type	freemem, %function

	.align	2

freemem:
	ldr	pc, =_freemem
