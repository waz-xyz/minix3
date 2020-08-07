	.arch armv7a
	.syntax unified

	.section .text

	.global symlink
	.type	symlink, %function

	.align	2

symlink:
	ldr	pc, =_symlink
