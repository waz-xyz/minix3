	.arch armv7a
	.syntax unified

	.section .text

	.global _exit
	.type	_exit, %function

	.align	2

_exit:
	ldr	pc, =__exit
