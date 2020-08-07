	.arch armv7a
	.syntax unified

	.section .text

	.global cfsetispeed
	.type	cfsetispeed, %function

	.align	2

cfsetispeed:
	ldr	pc, =_cfsetispeed
