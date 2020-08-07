	.arch armv7a
	.syntax unified

	.section .text

	.global mount
	.type	mount, %function

	.align	2

mount:
	ldr	pc, =_mount
