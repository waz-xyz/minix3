	.arch armv7a
	.syntax unified

	.section .text

	.global umount
	.type	umount, %function

	.align	2

umount:
	ldr	pc, =_umount
