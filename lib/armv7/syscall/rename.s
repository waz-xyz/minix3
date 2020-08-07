	.arch armv7a
	.syntax unified

	.section .text

	.global rename
	.type	rename, %function

	.align	2

rename:
	ldr	pc, =_rename
