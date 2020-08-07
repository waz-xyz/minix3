	.arch armv7a
	.syntax unified

	.section .text

	.global ioctl
	.type	ioctl, %function

	.align	2

ioctl:
	ldr	pc, =_ioctl
