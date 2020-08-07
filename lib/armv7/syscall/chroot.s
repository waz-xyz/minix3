	.arch armv7a
	.syntax unified

	.section .text

	.global chroot
	.type	chroot, %function

	.align	2

chroot:
	ldr	pc, =_chroot
