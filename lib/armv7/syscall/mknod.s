	.arch armv7a
	.syntax unified

	.section .text

	.global mknod
	.type	mknod, %function

	.align	2

mknod:
	ldr	pc, =_mknod
