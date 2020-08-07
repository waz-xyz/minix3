	.arch armv7a
	.syntax unified

	.section .text

	.global cfgetospeed
	.type	cfgetospeed, %function

	.align	2


cfgetospeed:
	ldr	pc, =_cfgetospeed
