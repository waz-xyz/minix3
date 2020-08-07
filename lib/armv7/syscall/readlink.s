	.arch armv7a
	.syntax unified

	.section .text

	.global readlink
	.type	readlink, %function

	.align	2

readlink:
	ldr	pc, =_readlink
