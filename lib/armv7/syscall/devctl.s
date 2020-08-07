	.arch armv7a
	.syntax unified

	.section .text

	.global devctl
	.type	devctl, %function

	.align	2

devctl:
	ldr	pc, =_devctl
