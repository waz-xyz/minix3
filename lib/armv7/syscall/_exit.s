	.arch armv7a
	.syntax unified

	.section .text

	.global __exit
	.type	__exit, %function

	.align	2

_exit:
	ldr	pc, =__exit
