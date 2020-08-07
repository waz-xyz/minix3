	.arch armv7a
	.syntax unified

	.section .text

	.global cfsetospeed
	.type	cfsetospeed, %function

	.align	2

cfsetospeed:
	ldr	pc, =_cfsetospeed
