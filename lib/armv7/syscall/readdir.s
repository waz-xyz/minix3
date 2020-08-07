	.arch armv7a
	.syntax unified

	.section .text

	.global readdir
	.type	readdir, %function

	.align	2

readdir:
	ldr	pc, =_readdir
