	.arch armv7a
	.syntax unified

	.section .text

	.global umask
	.type	umask, %function

	.align	2

umask:
	ldr	pc, =_umask
