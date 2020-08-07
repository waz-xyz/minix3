	.arch armv7a
	.syntax unified

	.section .text

	.global isatty
	.type	isatty, %function

	.align	2

isatty:
	ldr	pc, =_isatty
