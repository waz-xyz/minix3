	.arch armv7a
	.syntax unified

	.section .text

	.global seekdir
	.type	seekdir, %function

	.align	2

seekdir:
	ldr	pc, =_seekdir
