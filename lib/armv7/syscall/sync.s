	.arch armv7a
	.syntax unified

	.section .text

	.global sync
	.type	sync, %function

	.align	2

sync:
	ldr	pc, =_sync
