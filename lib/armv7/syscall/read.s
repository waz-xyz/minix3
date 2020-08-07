	.arch armv7a
	.syntax unified

	.section .text

	.global read
	.type	read, %function

	.align	2

read:
	ldr	pc, =_read
